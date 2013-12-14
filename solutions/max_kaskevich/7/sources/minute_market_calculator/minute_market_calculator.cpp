#include "minute_market_calculator.h"
#include <numeric>

using namespace minute_market;

minute_market::minute_market_calculator::minute_market_calculator( callback_type callback ) :
    callback_( callback )
{

}

minute_market::minute_market_calculator::~minute_market_calculator()
{
}

void add_trade( minute_datafeed& mdf, const trade_message_ptr& msg )
{
    mdf.high_prise = std::max( mdf.high_prise, msg->price() );
    mdf.low_price = std::min( mdf.low_price, msg->price() );
    mdf.volume += msg->volume();

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
    mdf.bid += msg->bid_volume();
    mdf.ask += msg->offer_volume();
}

void minute_market::minute_market_calculator::new_trade( const trade_message_ptr& msg )
{
    new_msg< const trade_message_ptr& >( msg, add_trade, add_first_trade,
        []( safe_minute_datafeed& smdf ) -> bool
        {
            if ( !smdf.have_trades )
            {
                smdf.have_trades = true;
                return true;
            }
            return false;
        });
}

void minute_market::minute_market_calculator::new_quote( const quote_message_ptr& msg)
{
    new_msg< const quote_message_ptr& >( msg, add_quote, add_quote,
        []( safe_minute_datafeed& smdf ) -> bool
        {
            if ( !smdf.have_quotes )
            {
                smdf.have_quotes = true;
                return true;
            }
            return false;
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

void minute_market::minute_market_calculator::dump()
{
    boost::unique_lock< boost::shared_mutex > write_lock( map_mtx_ );
    for( auto& stockname_pair: stock_name_map_ )
    {
        callback_( stockname_pair.second->mdf );
    }
}
