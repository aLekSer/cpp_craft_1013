#ifndef _minute_calculator_
#define _minute_calculator_
#include "boost/thread.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;
class minute_calculator
{
	ofstream& outp;
	boost::mutex calc_mtx;
public:
	minute_calculator(ofstream& output): outp ( output)
	{

	}
	~minute_calculator()
	{
	}

};
#endif //_minute_calculator_
