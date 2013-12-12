#include "minute_calculator.h"

void minute_calculator::push_trade( boost::shared_ptr<trade> trad )
{
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
		vals->minute = minute;
		vals.reset(new minute_extremums);
		push_trade(trad);
	}
	vals.reset();
}

void minute_calculator::push_quote( boost::shared_ptr<quote> quot )
{
	if(quot.use_count() == 0)
	{
		return;
	}
	if(uninit)
	{
		minute = quot->minute();
		uninit = false;
		vals->bid = 0;
		vals->ask = 0;
	}
	if(quot->minute() == minute)
	{
		minute = quot->get_time();
		strcpy(vals->stock_name, quot->security_symbol().c_str());
		vals->bid += quot->bid_volume(); 
		vals->ask += quot->offer_volume();

	}
	else if(quot->minute() > minute)
	{
		minute = quot->minute();
		shared_map shared_stats = extr;
		send_data(shared_stats);
		vals.reset(new minute_extremums);
		push_quote(quot);
	}
}

void minute_calculator::send_data( shared_map ) 
{

}
