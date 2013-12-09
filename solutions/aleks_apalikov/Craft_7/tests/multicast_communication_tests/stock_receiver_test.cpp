#include "test_registrator.h"
#include <string>
#include "Stock_receiver.h"
#include "config.h"
#include <sstream>
#include "market_data_processor.h"
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include "config.h"

typedef vector<ifstream*> vec_ifstr;
typedef vector<ifstream*>::iterator f_iter;
typedef boost::asio::ip::udp::endpoint endpoint;
typedef vector<endpoint>::iterator e_iter;
bool all_empty(vec_ifstr & tr, vec_ifstr & q)
{
	bool b = true;
	for (f_iter f = tr.begin(); f != tr.end(); f++)
	{
		b &= ((*f)->peek() == EOF);
	}
	for (f_iter f = q.begin(); f != q.end(); f++)
	{
		b &= ((*f)->peek() == EOF);
	}
	return b;
}

void clear_files(vec_ifstr & tr, vec_ifstr & q)
{
	for (f_iter f = tr.begin(); f != tr.end(); f++)
	{
		delete *f;
	}
	for (f_iter f = q.begin(); f != q.end(); f++)
	{
		delete *f;
	}
}
void async_udp::receiver_test()
{
	{
		cout << data_path << endl;
		stock_receiver sr;
		boost::asio::io_service service;
		boost::asio::ip::udp::endpoint endp( boost::asio::ip::address::from_string( "233.200.79.128" ), 62128 ); 
		boost::asio::ip::udp::socket socket( service, endp.protocol() );
		config c(data_path + string("config.ini"));
		typedef addresses::iterator iter;
		using namespace boost::asio::ip;

		vector<endpoint> trades;
		vec_ifstr trade_files;
		for (iter i = c.get_trades().begin(); i != c.get_trades().end(); ++i )
		{
			trades.push_back( endpoint(address::from_string( i->first ), i->second ));
			trade_files.push_back(new ifstream (string(data_path + i->first + ".udp").c_str()) );
		}
		vector<endpoint> quotes;
		vec_ifstr quote_files;
		for (iter q = c.get_quotes().begin(); q != c.get_quotes().end(); ++q )
		{
			quotes.push_back( endpoint(address::from_string( q->first ), q->second ));
			quote_files.push_back(new ifstream ( string(data_path + q->first + ".udp").c_str()) );
		}
	
		while (!all_empty(quote_files, trade_files))
		{
			f_iter tf = trade_files.begin();
			for (e_iter t = trades.begin(); t != trades.end(); t++, tf++ )
			{
				stringstream ss;
				if((*tf)->peek() != EOF)
				{
					message::read_block(ss, **tf);
					socket.send_to( boost::asio::buffer( ss.str().c_str(), ss.str().size() ), *t );
					ss.str("");
				}				
			}
			f_iter qf = quote_files.begin();
			for (e_iter q = quotes.begin(); q != quotes.end(); q++, qf++ )
			{
				stringstream ss;
				if((*qf)->peek() != EOF)
				{		
					message::read_block(ss, **qf);
					socket.send_to( boost::asio::buffer( ss.str().c_str(), ss.str().size() ), *q );
					ss.str("");
				}	
			}
			boost::this_thread::sleep_for( boost::chrono::nanoseconds( 100 ) );
			sr.wait_some_data();
		}

		clear_files(trade_files, quote_files);
		cout << "Total messages was parsed and passed: " << message::count << endl; 

		while( sr.wait_some_data() != -1 )
		{
			boost::this_thread::sleep_for( boost::chrono::nanoseconds( 1 ) );
		}
	}
	{
		stock_receiver sr("results2.txt");
		boost::asio::io_service service;
		boost::asio::ip::udp::endpoint endp( boost::asio::ip::address::from_string( "233.200.79.128" ), 62128 ); 
		boost::asio::ip::udp::socket socket( service, endp.protocol() );

		stringstream ss;
		ss << "\x01""EBEO A  003759557N:J_735AVB             0    AAAR B30000012127000000001D0000012137000000001     A   62TB00012130001 BB00012137001 "
			<<"\x1f""EDEO A  003759121P:J_428AINR  D00352000001 F00354300001 02"
			<<"\x1f""LDEO A  003759122N:J_432ALJR  F00124900003 D00125100001 02""\x03";
		string str = ss.str();
		socket.send_to( boost::asio::buffer( str.c_str(), str.size() ), endp );
		while( sr.wait_some_data() == -1 )
		{
			socket.send_to( boost::asio::buffer( str.c_str(), str.size() ), endp );
			boost::this_thread::sleep_for( boost::chrono::nanoseconds( 100 ) );
		}
		sr.stop();
	}

}

