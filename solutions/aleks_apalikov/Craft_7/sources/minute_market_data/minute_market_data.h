#ifndef _minute_market_data_
#define _minute_market_data_
#include "boost/thread.hpp"
#include "message_parser.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include "thread_safe_queue.h"
#include "../multicast_communication/message_parser.h"
#include "../minute_calculator/minute_calculator.h"
#include <map>
#include "boost/shared_ptr.hpp"
#include <string>
#include "../multicast_communication/config.h"
#include "../multicast_communication/Stock_receiver.h"

using namespace std;
class minute_market_data
{
	typedef boost::shared_ptr<ofstream> shared_fstream;
	typedef map<string, boost::shared_ptr<ofstream>> streams;
	minute_calculator* mc;
	stock_receiver sr;
	
	streams outp;
	thread_safe_queue<shared_map> que;
	boost::mutex mtx;
public:
	explicit minute_market_data()
	{
		outp.insert( make_pair("1", 
			new ofstream((string(data_path + "1.data")).c_str() )) );
		mc = new minute_calculator(&que);
		worker w(*mc);
		sr.add_callback(&w);

	}
	~minute_market_data()
	{
		while (!que.empty())
		{
			que.pop();
		}
		delete mc;
		close();
	}
	void get_trade(boost::shared_ptr<trade> t)
	{
		mc->push_trade(t);
	}
	void get_quote(boost::shared_ptr<quote> q)
	{
		mc->push_quote(q);
	}
	
	int process_one()
	{
		shared_map m;
		que.pop(m);
		for (map_extr::iterator i = m->begin(); i != m->end(); ++i)
		{
			string str = i->first;
			shared_extremums vals = i->second;
			write(vals, *outp["1"]);
		}
	}
	void write(shared_extremums e, ofstream& fs )
	{
		if(!fs.is_open())
		{
			return ;
		}
		fs << e->minute << " " << e->stock_name << " ";
		if(e->open_price > 0)
		{
			fs << e->open_price << " ";
		}
		else 
			fs << "  ";

		if(e->high_price > 0)
		{
			fs << e->high_price << " ";
			fs << e->low_price << " ";
			fs << e->close_price << " ";
		}
		else
			fs << "\t\t";
		if(e->volume > 0)
		{
			fs << e->volume << " ";
		}
		else
			fs << "  ";
		if(e->bid > 0)
		{
			fs << e->bid << " ";
		}
		else
			fs << "  ";

		if(e->ask > 0)
		{
			fs << e->ask << " ";
		}
		else
			fs << "  ";		
	}
	void close()
	{
		for (streams::iterator i = outp.begin(); i != outp.end(); ++i)
		{
			i->second->close();
		}
	}

};
#endif //_minute_market_data_
