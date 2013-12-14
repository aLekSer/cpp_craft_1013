#include "Stock_receiver.h"

stock_receiver::stock_receiver(char * str): c(config (data_path + string("config.ini"))),
	processor ( market_data_processor(str) )
{
	c.get_trades();
	c.trade_threads();
	c.quote_threads();
	trad_co = new callable_obj(this, false);
	quot_co = new callable_obj(this, true);

	init_services( quote_services, c, true);	
	init_services(trade_services, c, false);

	init_listeners(true);
	init_listeners(false);
	deleted[0] = deleted[1] = false;

	size_t denom = c.get_trades().size();
	for(size_t i = 0; i < c.trade_threads() && denom != 0; i++ )
	{
		threads.create_thread(boost::bind(&stock_receiver::service_run, this, trade_services[i % denom]));
	}

	denom = c.get_quotes().size();
	for(size_t i = 0; i < c.quote_threads() && denom != 0; i++ )
	{
		threads.create_thread(boost::bind(&stock_receiver::service_run, this, quote_services[i % denom]));
	}
}

stock_receiver::~stock_receiver(void)
{
	if(trad_co)
		delete trad_co;
	if(quot_co)
		delete quot_co;
	stop();
}

void stock_receiver::init_services( vector<shared_service> & vs, config & c, const bool quotes )
{
	size_t siz =
	quotes ? c.quote_threads() : c.trade_threads();
	if(siz == 0)
	{
		return;
	}
	vs.reserve(siz);
	for(size_t i = 0; i != siz; i++)
	{
		shared_service sp;
		sp.reset(new boost::asio::io_service);
		vs.push_back(sp);
	}
}

void stock_receiver::init_listeners( const bool quotes )
{
	listeners_vec  & lv = quotes ? quote_listeners :  trade_listeners;
	vector<shared_service> & vs = quotes ? quote_services : trade_services;
	size_t siz = quotes ? c.quote_ports() : c.trade_ports();
	callable_obj * co = quotes ? quot_co : trad_co;
	const addresses & a = quotes ? c.get_quotes() : c.get_trades();
	if (a.size() == 0)
	{
		return;
	}
	lv.reserve(a.size());
	for(size_t i = 0; i < siz; i++)
	{
		udp_listener* sp = new udp_listener(*vs[i], a[i].first, a[i].second, co);
		lv.push_back (sp);
	}
}

int stock_receiver::wait_some_data()
{
	int ret_val = -1;
	for(size_t i = 0; i < trade_listeners.size(); ++i) { 
		if (trade_listeners[i]->messages().size() > 0 )
		{
			vector_messages msgs;
			boost::shared_ptr<string> str;
			{
				boost::mutex::scoped_lock lock (trade_listeners[i]->protect_messages() );
				str = trade_listeners[i]->messages_pop() ;
			}
			message::divide_messages(msgs, str, false);
			for (vector_messages::iterator it = msgs.begin(); it != msgs.end(); it++)
			{
				boost::shared_ptr<trade> sp = boost::static_pointer_cast<trade, message>(*it);
				(*work)(sp);
				sp.reset();
			}
			if(processor.wr_trades(msgs) != 0)
				ret_val = static_cast<int>( i );
			break;
		}
	}

	for(size_t i = 0; i < quote_listeners.size(); ++i) { 
		if (quote_listeners[i]->messages().size() > 0 )
		{
			vector_messages msgs;
			boost::shared_ptr<string> str;
			{
				boost::mutex::scoped_lock lock (quote_listeners[i]->protect_messages() );
				str = quote_listeners[i]->messages_pop();
			}
			message::divide_messages(msgs,  str, true);
			for (vector_messages::iterator it = msgs.begin(); it != msgs.end(); it++)
			{
				boost::shared_ptr<quote> sp = boost::static_pointer_cast<quote, message>(*it);
				(*work)(sp);
				sp.reset();
			}
			if(processor.wr_quotes(msgs) != 0)
				return static_cast<int>( i );
			else return ret_val;
		}
	}
	processor.flush();
//	cout<<"Msgs clear!"<<endl;
	return ret_val;
}

void stock_receiver::service_run(shared_service serv)
{
	serv->run();
}

void stock_receiver::stop()
{
	vector<shared_service>::iterator it;
	work->stop();
	processor.close();
	for (it = quote_services.begin(); it != quote_services.end(); it++)
	{
		(*(it))->stop();
	}
	for (it = trade_services.begin(); it != trade_services.end(); it++)
	{
		(*(it))->stop();
	}
	threads.join_all();
	del_listeners(true);
	del_listeners(false);
}

void stock_receiver::add_callback( worker* w, minute_data_call* mc )
{
	work = w;
	mdc = mc;
}

void stock_receiver::del_listeners(bool quotes)
{
	listeners_vec  & lv = quotes ? quote_listeners :  trade_listeners;
	vector<shared_service> & vs = quotes ? quote_services : trade_services;
	size_t siz = quotes ? c.quote_ports() : c.trade_ports();
	const addresses & a = quotes ? c.get_quotes() : c.get_trades();
	bool & del = quotes ? deleted[0]: deleted[1]; 
	if (a.size() == 0)
	{
		return;
	}
	if(del == true)
	{
		return;
	}
	for(size_t i = 0; i < siz; i++)
	{
		delete lv.back ();
		lv.pop_back();
	}
	del = true;
	
}

void stock_receiver::write_buf( boost::shared_ptr<string> str, bool quotes )
{
	vector_messages msgs;
	try
	{
		message::divide_messages(msgs, str, quotes);
	}
	catch (const exception & e)
	{
		cout << "Exception while udp buffer reading"<< e.what() << endl;
	}
	for (vector_messages::iterator it = msgs.begin(); it != msgs.end(); it++)
	{
		if(!quotes)
		{
			boost::shared_ptr<trade> sp = boost::static_pointer_cast<trade, message>(*it);
			(*work)(sp);
			(*mdc) ();
			sp.reset();
		}
		else
		{
			boost::shared_ptr<quote> sp = boost::static_pointer_cast<quote, message>(*it);
			(*work)(sp);
			(*mdc) ();
			sp.reset();
		}
	}
	quotes ? processor.wr_quotes(msgs): processor.wr_trades(msgs) ;
}


void minute_data_call::operator()() /*for writing */
{
	mmd->process_one();
}
