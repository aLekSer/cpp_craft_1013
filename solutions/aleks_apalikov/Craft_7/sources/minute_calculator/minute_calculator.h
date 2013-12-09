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
	}
	~minute_calculator()
	{
	}
	void push_trade(const boost::shared_ptr<trade> trad);
	void push_quote(const boost::shared_ptr<quote> quot);
};
#endif //_minute_calculator_
