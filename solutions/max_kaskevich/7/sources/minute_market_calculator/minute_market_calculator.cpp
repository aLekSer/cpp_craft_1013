#include "minute_market_calculator.h"
#include <numeric>


minute_market::minute_market_calculator::minute_market_calculator( callback_type callback ) :
    callback_( callback )
{

}


void minute_market::minute_market_calculator::new_trade( const trade_message_ptr& msg )
{

}

void minute_market::minute_market_calculator::new_quote( const quote_message_ptr& )
{

}

void minute_market::minute_market_calculator::add_first_trade(  minute_datafeed& mdf, const trade_message_ptr& msg )
{
    mdf = minute_datafeed();
    mdf.open_prise = msg->price();
    mdf.minute = msg->time();
    mdf.low_price = std::numeric_limits< double >::max();
    add_trade( mdf, msg );
}

void minute_market::minute_market_calculator::add_trade(  minute_datafeed& mdf, const trade_message_ptr& msg )
{
    std::max( mdf.high_prise, msg->price() );
    std::min( mdf.low_price, msg->price() );
    mdf.volume += msg->volume();
    std::memcpy( mdf.stock_name, msg->security_symbol().c_str(), msg->security_symbol().length() );

    mdf.close_prise = msg->price(); // save price of last added trade
}

void minute_market::minute_market_calculator::add_quote(  minute_datafeed& mdf, const quote_message_ptr& msg)
{
    mdf.bid += msg->bid_price();
    mdf.ask += msg->offer_price();
}
