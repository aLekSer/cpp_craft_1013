#include "minute_market_data.h"


void minute_market_data::run_proc()
{
	while (to_run)
	{
		sr->wait_some_data();
		boost::this_thread::sleep_for(boost::chrono::nanoseconds(10));
	}
}

minute_market_data::minute_market_data()
{
	to_run = true;
	mc = new minute_calculator(&que);
	sr = new stock_receiver;
	w = new worker(mc);
	mdc = new minute_data_call(this);
	sr->add_callback(w, mdc);
}

void minute_market_data::stop()
{
	to_run = false;
	sr->stop();
	while (process_one() != -1)
	{
	}
}

minute_market_data::~minute_market_data()
{
	while (!que.empty())
	{
		que.pop();
	}
	close();
	erase_output();
	if (sr)
	{
		delete sr;
	}
	if (mc)
	{
		delete mc;
		mc = NULL;
	}
	if (w)
	{
		delete w;
	}
	if (mdc)
	{
		delete mdc;
	}
}

void minute_market_data::erase_output()
{
	for(streams::iterator i = outp.begin(); i != outp.end(); i++)
	{
		i->second.reset();
	}
}
