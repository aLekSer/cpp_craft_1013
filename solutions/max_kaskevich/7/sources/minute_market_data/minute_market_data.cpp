#include "minute_market_data.h"
#include <string>


minute_market::minute_market_data::minute_market_data( const std::string& dir_path ) :
    dir_path_( dir_path ),
//    calculator_( boost::bind( &minute_market::minute_market_data::on_minute_data_feed, this ) )
    calculator_( [this] (const minute_datafeed& data){ this->on_minute_data_feed( data ); } ),
    stopped_( false )
{
        
}

minute_market::minute_market_data::~minute_market_data()
{
    stop();
}

void minute_market::minute_market_data::new_trade( const trade_message_ptr& msg )
{
    engine_.add_task(
        boost::bind( &minute_market::minute_market_calculator::new_trade, &calculator_, msg ) );
}

void minute_market::minute_market_data::new_quote( const quote_message_ptr& msg )
{
    engine_.add_task(
        boost::bind( &minute_market::minute_market_calculator::new_quote, &calculator_, msg ) );
}

void minute_market::minute_market_data::on_minute_data_feed( const minute_datafeed& data )
{
    boost::mutex::scoped_lock lock( output_mtx_ );
    std::ofstream& output = outputs_[ data.stock_name ];
    if( !output.is_open() )
    {
        std::string path = dir_path_ + "\\";
        path += data.stock_name;
        path += ".dat";
        output.open( path );
    }
    output << data;
}

void minute_market::minute_market_data::stop()
{
    if( stopped_ )
    {
        return;
    }
    engine_.stop();
    calculator_.dump();
    stopped_ = true;
}
