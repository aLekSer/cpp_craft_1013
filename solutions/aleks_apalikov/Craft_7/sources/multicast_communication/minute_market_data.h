#ifndef _minute_market_data_
#define _minute_market_data_
#include "boost/thread.hpp"
#include "Stock_receiver.h"
#include "message_parser.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include "thread_safe_queue.h"
#include "message_parser.h"
#include "minute_calculator.h"
#include <map>
#include "boost/shared_ptr.hpp"
#include <string>
#include "config.h"
#include "boost/chrono.hpp"

using namespace std;
class stock_receiver;
class worker;
class minute_data_call;
class minute_market_data
{
	typedef boost::shared_ptr<ofstream> shared_fstream;
	typedef map<string, boost::shared_ptr<ofstream>> streams;
	minute_calculator* mc;
	stock_receiver* sr;
	worker* w;
	minute_data_call* mdc;
	
	streams outp;
	boost::thread net;
	boost::thread writer;
	thread_safe_queue<shared_map> que;
	bool to_run;
	boost::mutex mtx;
	void run_proc();
public:
	explicit minute_market_data();
	~minute_market_data();
	void get_trade(boost::shared_ptr<trade> t)
	{
		mc->push_trade(t);
	}
	void get_quote(boost::shared_ptr<quote> q)
	{
		mc->push_quote(q);
	}
	void process_func()
	{
		while(to_run)
		{
			process_one();
			boost::this_thread::sleep_for(boost::chrono::nanoseconds(100));
		}
	}
	int process_one()
	{
		shared_map m;
		if(!que.pop(m))
		{
			return -1;
		}
		for (map_extr::iterator i = m->begin(); i != m->end(); ++i)
		{
			streams::iterator search = outp.find(i->first );

			string str = i->first;
			shared_extremums vals = i->second;
			if (search != outp.end())
			{
				write(vals, *(search->second));
				search->second->flush();
			}
			else
			{
				boost::shared_ptr<ofstream> sh_of (new ofstream(string(data_path 
					+ "/stocks/" + str + ".dat").c_str(),
					ios::binary | ios::out | ios::app));
				outp.insert(make_pair(str, sh_of));
				write(vals, *sh_of);
				sh_of->flush();
			}
		}
		return 0;
	}
	void write(shared_extremums e, ofstream& fs )
	{
		if(!fs.is_open())
		{
			return ;
		}
//		write_binary(fs, *e);
		write_binary(fs, e->minute);
		write_binary(fs, e->stock_name, 16);
		write_binary(fs, e->open_price);
		write_binary(fs, e->high_price);
		write_binary(fs, e->low_price);
		write_binary(fs, e->close_price);
		write_binary(fs, e->volume);
		write_binary(fs, e->bid);
		write_binary(fs, e->ask);
		fs.flush();
	}
protected:
	void run()
	{
		net = boost::thread(boost::bind(&minute_market_data::run_proc, this));
		writer = boost::thread(boost::bind(&minute_market_data::process_func, this));
	}
public:
	void stop();
	void close()
	{
		for (streams::iterator i = outp.begin(); i != outp.end(); ++i)
		{
			i->second->close();
		}
	}
	void erase_output();
};
#endif //_minute_market_data_
