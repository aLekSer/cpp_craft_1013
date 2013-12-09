#ifndef _stock_receiver_
#define _stock_receiver_
#include "config.h"
#include "boost/shared_ptr.hpp"
#include "boost/asio.hpp"
#include "udp_listener.h"
#include <vector>
#include "boost/thread.hpp"
#include "message_parser.h"
#include <stdexcept>
#include "market_data_processor.h"

using namespace std;
using namespace async_udp;

class stock_receiver
{
	typedef boost::shared_ptr<boost::asio::io_service> shared_service;
	vector<shared_service> quote_services;
	vector<shared_service> trade_services;
	boost::thread_group threads;
	typedef vector<boost::shared_ptr<udp_listener>> listeners_vec;
	listeners_vec quote_listeners;
	listeners_vec trade_listeners;
	market_data_processor processor;
	static void init_services(vector<shared_service> & vs, config & c, const bool quotes);
	void init_listeners( bool quotes);
	config c;
public:
	int wait_some_data();
	void service_run(shared_service serv);
	stock_receiver(char * str = "");
	void stop();
	~stock_receiver(void);
};
#endif
