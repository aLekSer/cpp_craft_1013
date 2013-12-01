#include "minute_market_data.h"
#include <string>


minute_market::minute_market_data::minute_market_data( const std::string& dir_path,
                                                       minute_market_calculator& calculator ) :
    dir_path_( dir_path ),
    calculator_( calculator )
{
        
}

void minute_market::minute_market_data::new_trade( const trade_message_ptr& msg )
{

}

void minute_market::minute_market_data::new_quote( const quote_message_ptr& msg )
{

}

void minute_market::minute_market_data::on_minute_data_feed( const minute_datafeed& data )
{

}
