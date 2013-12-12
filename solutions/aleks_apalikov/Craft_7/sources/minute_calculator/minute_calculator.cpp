#include "minute_calculator.h"

void minute_calculator::push_trade( boost::shared_ptr<trade> trad )
{
//	boost::mutex::scoped_lock lock(calc_mtx);
	bool insert = false;
	if(trad.use_count() == 0)
	{
		return;
	}
	if(uninit)
	{
		minute = trad->minute();
		uninit = false;
		extr.reset(new map_extr);
	}
	if(trad->minute() == minute)
	{
		char st [sn_size];
		strcpy(st, trad->security_symbol().c_str());

		map_extr::iterator i = extr->find(string (st));
		if(i != extr->end())
		{
			vals = (*i).second;
		}
		else
		{
			vals.reset(new minute_extremums);
			insert = true;
		}
		vals->minute = trad->minute();
		strcpy(vals->stock_name, st);
		double price = trad->price() / trad->denom();
		if(trad->msec() < vals->first_msec)
		{
			vals->first_msec = trad->msec();
			vals->open_price = price;
		}
		if(trad->msec() >= vals->last_msec)
		{
			vals->last_msec = trad->msec();
			vals->close_price = price;
		}
		if(price < vals->low_price || vals->low_price < 0)
		{
			vals->low_price = price;
		}
		if(price > vals->high_price)
		{
			vals->high_price = price;
		}
		vals->volume += trad->volume();
		if(insert)
		{
			extr->insert(make_pair(string(vals->stock_name), vals));
		}
	}
	else if(trad->minute() > minute)
	{
		shared_map shared_stats;
		shared_stats = extr;
		send_data(shared_stats);
		extr.reset(new map_extr);
		minute = trad->minute();
		push_trade(trad);
	}
	vals.reset();
}

void minute_calculator::push_quote( boost::shared_ptr<quote> quot )
{
	//	boost::mutex::scoped_lock lock(calc_mtx);
	bool insert = false;
	if(quot.use_count() == 0)
	{
		return;
	}
	if(uninit)
	{
		minute = quot->minute();
		uninit = false;
		extr.reset(new map_extr);
	}
	if(quot->minute() == minute)
	{
		char st [sn_size];
		minute = quot->get_time();
		strcpy(st, quot->security_symbol().c_str());
		map_extr::iterator i = extr->find(string (st));
		if(i != extr->end())
		{
			vals = (*i).second;
		}
		else
		{
			vals.reset(new minute_extremums);
			insert = true;
		}
		vals->minute = quot->minute();
		strcpy(vals->stock_name, st);
		vals->bid += quot->bid_volume(); 
		vals->ask += quot->offer_volume();

		if(insert)
		{
			extr->insert(make_pair(string(vals->stock_name), vals));
		}

	}
	else if(quot->minute() > minute)
	{
		shared_map shared_stats;
		shared_stats = extr;
		send_data(shared_stats);
		extr.reset(new map_extr);
		minute = quot->minute();
		push_quote(quot);
	}
	vals.reset();
}
void minute_calculator::send_data( shared_map stat) 
{
	que->push(stat);
}
