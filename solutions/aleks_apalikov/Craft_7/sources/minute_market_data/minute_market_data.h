#ifndef _minute_market_data_
#define _minute_market_data_
#include "boost/thread.hpp"
#include "message_parser.h"
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;
class minute_market_data
{
	ofstream& outp;
	boost::mutex mtx;
public:
	minute_market_data(ofstream& output): outp ( output)
	{

	}
	~minute_market_data()
	{
	}
	void close()
	{
//		outp.close();
	}

};
#endif //_minute_market_data_
