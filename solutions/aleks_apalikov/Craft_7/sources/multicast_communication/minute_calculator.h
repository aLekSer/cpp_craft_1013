#ifndef _minute_calculator_
#define _minute_calculator_
#include "boost/thread.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include "../multicast_communication/message_parser.h"
#include <vector>
#include <stdint.h>
#include "boost/shared_ptr.hpp"
#include "../minute_market_data/thread_safe_queue.h"

enum{
	sn_size = 16
};
struct minute_extremums
{
	uint32_t minute;
	char stock_name[sn_size];
	double open_price;
	double high_price;
	double low_price;
	double close_price;
	double volume;
	double bid;
	double ask;

	//extra fields
	uint32_t first_msec; // in minute
	uint32_t last_msec;
	minute_extremums ()
	{
		close_price = high_price = open_price = low_price = -1; // for checking before outputting
		volume = 0;
		ask = bid = 0; // check for == 0
		first_msec = 60001;
		last_msec = 0;
	}
};
using namespace std;

typedef vector<trade> vec_tr;
typedef vector<quote> vec_q;
typedef boost::shared_ptr<minute_extremums> shared_extremums;
typedef std::map<string, shared_extremums> map_extr;
typedef boost::shared_ptr<map_extr> shared_map;
class minute_calculator
{ 
	shared_extremums vals;
	shared_map extr;
	vec_tr trades; //try to add thread safe queue to read at both ends or use pipes
	vec_q quotes;
	boost::mutex calc_mtx;
	bool uninit;
	uint32_t minute;
	thread_safe_queue<shared_map>* que;
	void send_data(shared_map); //send to market_data
public:
	explicit minute_calculator(thread_safe_queue<shared_map>* q)
	{
		que = q;
		uninit = true;		
	}
	~minute_calculator()
	{
	}
	void push_trade( boost::shared_ptr<trade> trad);
	void push_quote( boost::shared_ptr<quote> quot);
	shared_map get_stat()
	{
		return extr;
	}
};
#endif //_minute_calculator_
