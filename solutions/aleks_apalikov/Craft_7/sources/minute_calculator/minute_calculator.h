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


struct minute_extremums
{
	uint32_t minute;
	char stock_name[16];
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
};
using namespace std;

typedef vector<trade> vec_tr;
typedef vector<quote> vec_q;
class minute_calculator
{
	boost::shared_ptr<minute_extremums> vals;
	vec_tr trades; //try to add thread safe queue to read at both ends or use pipes
	vec_q quotes;
	boost::mutex calc_mtx;
	bool uninit;
	uint32_t minute;
	void send_data(boost::shared_ptr<minute_extremums>); //send to market_data
public:
	minute_calculator()
	{
		uninit = true;
		vals.reset (new minute_extremums);
		vals->close_price = vals->high_price = vals->open_price 
			= vals->low_price = -1; // for checking before outputting
		vals->volume = 0;
		vals->ask = vals->bid = 0; // check for == 0
		vals->first_msec = 60001;
		vals->last_msec = 0;
		
	}
	~minute_calculator()
	{
	}
	void push_trade( boost::shared_ptr<trade> trad);
	void push_quote( boost::shared_ptr<quote> quot);
	boost::shared_ptr<minute_extremums> get_vals()
	{
		return vals;
	}
};
#endif //_minute_calculator_
