#include "minute_market_calculator.h"
#include <numeric>

using namespace minute_market;

minute_market::minute_market_calculator::minute_market_calculator( callback_type callback ) :
    callback_( callback )
{

}

void add_trade( minute_datafeed& mdf, const trade_message_ptr& msg )
{
    mdf.high_prise = std::max( mdf.high_prise, msg->price() );
    mdf.low_price = std::min( mdf.low_price, msg->price() );
    size_t stock_name_legnth = msg->security_symbol().length();
    mdf.volume += msg->volume();
    std::memcpy( mdf.stock_name, msg->security_symbol().c_str(), stock_name_legnth );
    mdf.stock_name[ stock_name_legnth ] = '\0';

    mdf.close_prise = msg->price(); // save price of last added trade
}

void add_first_trade( minute_datafeed& mdf, const trade_message_ptr& msg )
{
    mdf.open_prise = msg->price();
    mdf.high_prise = std::numeric_limits< double >::min();
    mdf.low_price = std::numeric_limits< double >::max();
    add_trade( mdf, msg );
}

void add_quote( minute_datafeed& mdf, const quote_message_ptr& msg)
{
    mdf.bid += msg->bid_price();
    mdf.ask += msg->offer_price();
}

void minute_market::minute_market_calculator::new_trade( const trade_message_ptr& msg )
{
    new_msg< const trade_message_ptr& >( msg, add_trade, add_first_trade,
        []( const minute_datafeed& mdf ) -> bool
        {
            return mdf.open_prise == 0.0;
        });
}

void minute_market::minute_market_calculator::new_quote( const quote_message_ptr& msg)
{
    new_msg< const quote_message_ptr& >( msg, add_quote, add_quote,
        []( const minute_datafeed& mdf ) -> bool
        {
            return mdf.bid == 0.0;
        });
}

std::ostream& minute_market::operator<<( std::ostream& output, const minute_datafeed& mdf )
{
    output.write( reinterpret_cast< const char* >( &mdf.minute ), sizeof( mdf.minute) );
    output.write( mdf.stock_name, 16 );
    output.write( reinterpret_cast< const char* >( &mdf.open_prise ), sizeof( mdf.open_prise) );
    output.write( reinterpret_cast< const char* >( &mdf.high_prise ), sizeof( mdf.high_prise) );
    output.write( reinterpret_cast< const char* >( &mdf.low_price ), sizeof( mdf.low_price) );
    output.write( reinterpret_cast< const char* >( &mdf.close_prise ), sizeof( mdf.close_prise) );
    output.write( reinterpret_cast< const char* >( &mdf.volume ), sizeof( mdf.volume) );
    output.write( reinterpret_cast< const char* >( &mdf.bid ), sizeof( mdf.bid) );
    output.write( reinterpret_cast< const char* >( &mdf.ask ), sizeof( mdf.ask) );
    return output;
}

