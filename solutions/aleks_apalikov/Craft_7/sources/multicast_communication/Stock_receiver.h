#ifndef _stock_receiver_
#define _stock_receiver_
#include "config.h"
#include "boost/shared_ptr.hpp"
#include "boost/asio.hpp"
#include "udp_listener.h"
#include <vector>
#include "minute_calculator.h"
#include "boost/thread.hpp"
#include "message_parser.h"
#include <stdexcept>
#include "market_data_processor.h"
#include "minute_market_data.h"

using namespace std;
using namespace async_udp;

//typedef void (* trade_func) (boost::shared_ptr<trade>);
//typedef void (* quote_func) (boost::shared_ptr<quote>);
class worker
{
	minute_calculator* minc;
public:
	explicit worker(minute_calculator* mc): minc (mc)
	{

	}
	void operator() (boost::shared_ptr<trade> t)
	{
		minc->push_trade(t);

	}
	void operator() (boost::shared_ptr<quote> q)
	{
		minc->push_quote(q);
	}
};
class minute_market_data;
class minute_data_call
{
	minute_market_data* mmd;
public:
	explicit minute_data_call(minute_market_data* minut_data)
	{
		mmd = minut_data;
	}
	void operator() (); //for writing;
};
class udp_listener;
typedef vector<udp_listener*> listeners_vec;
class callable_obj;
class stock_receiver
{
	typedef boost::shared_ptr<boost::asio::io_service> shared_service;
	vector<shared_service> quote_services;
	vector<shared_service> trade_services;
	boost::thread_group threads;
	callable_obj* trad_co;
	callable_obj* quot_co;
	listeners_vec quote_listeners;
	listeners_vec trade_listeners;
	market_data_processor processor;
	worker* work;
	minute_data_call* mdc;
	static void init_services(vector<shared_service> & vs, config & c, const bool quotes);
	void init_listeners( bool quotes);
	config c;
	bool deleted[2];
public:
	int wait_some_data();
	void add_callback(worker* work, minute_data_call* mc);
	void write_buf( boost::shared_ptr<string> str, bool quotes );
	void service_run(shared_service serv);
	stock_receiver(char * str = "");
	void stop();
	~stock_receiver(void);
	void del_listeners(bool quotes);
};
#endif
