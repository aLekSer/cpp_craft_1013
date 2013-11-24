#include "async_writer.h"
#include <fstream>


multicast_communication::async_writer::async_writer( std::ostream& output ) :
    output_( output ),
    quote_thread_( boost::bind( &multicast_communication::async_writer::quote_proc, this ) ),
    trade_thread_( boost::bind( &multicast_communication::async_writer::trade_proc, this ) )
{
}


multicast_communication::async_writer::~async_writer(void)
{
    join();
}

void multicast_communication::async_writer::new_trade( const trade_message_ptr& msg )
{
    trade_queue_.push( msg );
}

void multicast_communication::async_writer::new_quote( const quote_message_ptr& msg )
{
    quote_queue_.push( msg );
}

void multicast_communication::async_writer::quote_proc()
{
    quote_message_ptr msg;
    while( output_ && quote_queue_.wait_pop( msg ) )
    {
        boost::mutex::scoped_lock lock( mtx_ );
        output_ << *( msg );
    }
}

void multicast_communication::async_writer::trade_proc()
{
    trade_message_ptr msg;
    while( output_ && trade_queue_.wait_pop( msg ) )
    {
        boost::mutex::scoped_lock lock( mtx_ );
        output_ << *( msg );
    }
}

void multicast_communication::async_writer::join()
{
    quote_thread_.join();
    trade_thread_.join();
}
