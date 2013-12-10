#include "minute_calculator.h"

void minute_calculator::push_trade( boost::shared_ptr<trade> trad )
{
	if(uninit)
	{
		minute = trad->minute();
		uninit = true;
		vals->volume = 0;
	}
	if(trad->minute() == minute)
	{
		minute = trad->get_time();
		strcpy(vals->stock_name, trad->security_symbol().c_str());
		double price = trad->price() / trad->denom();
		if(trad->msec() < vals->first_msec)
		{
			vals->first_msec = trad->msec();
			vals->open_price = price;
		}
		if(trad->msec() < vals->last_msec)
		{
			vals->last_msec = trad->msec();
			vals->close_price = price;
		}
		if(price < vals->low_price)
		{
			vals->low_price = price;
		}
		if(price > vals->high_price)
		{
			vals->high_price = price;
		}
		vals->volume = trad->volume();
	}
	else if(trad->minute() > minute)
	{
		minute = trad->minute();
		boost::shared_ptr<minute_extremums> shared_stats;
		shared_stats = vals;
		send_data(shared_stats);
		vals.reset(new minute_extremums);
		push_trade(trad);
	}
}

void minute_calculator::push_quote( boost::shared_ptr<quote> quot )
{
	if(uninit)
	{
		minute = quot->minute();
		uninit = true;
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
		boost::shared_ptr<minute_extremums> shared_stats;
		shared_stats = vals;
		send_data(shared_stats);
		vals.reset(new minute_extremums);
		push_quote(quot);
	}
}

void minute_calculator::send_data( boost::shared_ptr<minute_extremums> ) 
{

}
