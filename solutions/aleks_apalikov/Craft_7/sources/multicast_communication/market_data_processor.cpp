#include "market_data_processor.h"

int market_data_processor::wr_trade( shared_trade trad )
{
	if(!outp.is_open())
	{
		cout<< "market_data_processor error" << endl;
	}
	outp << "T " << std::fixed << trad->security_symbol() << " " ;
outp << std::setprecision(2); 
	if(trad->divider() < 0)
		outp << " ";
	else
		outp << ( trad->price() / trad->divider()) << " ";
	outp << std::setprecision(1) << trad->volume() << std::endl;
	return 0;
}

int market_data_processor::wr_quote( shared_quote quot )
{
	if(!outp.is_open())
	{
		cout<< "market_data_processor error" << endl;
	}
	outp << "Q " << std::fixed << quot->security_symbol() << " ";
outp << std::setprecision(2);
 if(quot->bid_divider() > 0) 
		outp << ( quot->bid_price() / quot->bid_divider() ) << " ";
	else 
		outp << " ";
	outp << std::setprecision(1) << quot->bid_volume()  << " " ;
	if(quot->offer_divider() > 0)	
		outp << std::setprecision(2) << ( quot->offer_price() / quot->offer_divider() ) << " ";
	else
		outp << " ";
	outp << std::setprecision(1) << quot->offer_volume() << std::endl;
	return 0;
}

int market_data_processor::wr_trades( vector_messages& msgs )
{
	boost::mutex::scoped_lock lock(mtx);
	size_t siz = msgs.size();
	while (!msgs.empty())
	{
		wr_trade(boost::static_pointer_cast<trade, message>(msgs.back()));
		msgs.pop_back();
	}
	return  static_cast<int>( siz );
}

int market_data_processor::wr_quotes( vector_messages& msgs )
{
	boost::mutex::scoped_lock lock(mtx);
	size_t siz = msgs.size();
	while (!msgs.empty())
	{
		wr_quote(boost::static_pointer_cast<quote, message>(msgs.back()));
		msgs.pop_back();
	}
	return  static_cast<int>( siz );
}

