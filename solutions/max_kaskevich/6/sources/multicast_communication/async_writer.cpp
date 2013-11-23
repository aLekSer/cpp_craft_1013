#include "async_writer.h"
#include <fstream>


multicast_communication::async_writer::async_writer( std::ostream& output ) :
    output_( output ),
    queue_(),
    thread_( boost::bind( &multicast_communication::async_writer::proc, this ) ) 
{
}


multicast_communication::async_writer::~async_writer(void)
{
}

void multicast_communication::async_writer::new_trade( const trade_message_ptr& msg )
{

}

void multicast_communication::async_writer::new_quote( const quote_message_ptr& msg )
{
    queue_.push( msg );
}

void multicast_communication::async_writer::proc()
{
    quote_message_ptr msg;
    while( queue_.wait_pop( msg ) && output_ << *( msg ) )
    {}
}

void multicast_communication::async_writer::join()
{
    thread_.join();
}
