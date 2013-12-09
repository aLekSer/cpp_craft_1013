#include "minute_calculator.h"

void minute_calculator::push_trade( const boost::shared_ptr<trade> trad )
{
	if(uninit)
	{
		minute = (trad->get_time() >> 8) * 60 + trad->get_time() | 0xFF;
		uninit = true;
	}
	if(trad->get_time() > minute)
	{
		minute = trad->get_time();
		strcpy(vals.stock_name, trad->security_symbol().c_str());
		if(trad->msec() < vals.first_msec)
		{
			vals.first_msec = trad->msec();
			vals.open_price = trad->price();
		}
		if(trad->msec() < vals.last_msec)
		{
			vals.last_msec = trad->msec();
			vals.close_price = trad->price();
		}
		if(trad->price() < vals.low_price)
		{
			vals.low_price = trad->price();
		}
		if(trad->price() > vals.high_price)
		{
			vals.high_price = trad->price();
		}
		vals.volume = trad->volume();
		for(vec_tr::iterator i = trades.begin(); i != trades.end(); i++)
		{

		}
	}
}

void minute_calculator::push_quote( const boost::shared_ptr<quote> quot )
{
	if(uninit)
	{
		minute = (quot->get_time() >> 8) * 60 + quot->get_time() | 0xFF;
		uninit = true;
	}
	if(quot->get_time() > minute)
	{
		minute = quot->get_time();
	}

}
