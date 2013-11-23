#include "market_data_receiver.h"
#include "udp_listener.h"

multicast_communication::market_data_receiver::market_data_receiver( const size_t trade_thread_size,
                                                                     const size_t quote_thread_size,
                                                                     const ports& trade_ports,
                                                                     const ports& quote_ports,
                                                                     market_data_processor& processor) :
    trade_thread_size_( trade_thread_size ),
    quote_thread_size_( quote_thread_size ),
    trade_ports_( trade_ports ),
    quote_ports_( quote_ports ),
    processor_( processor )
{
    if( trade_thread_size < trade_ports.size() )
    {
        throw std::logic_error( "too little threads for trade ports" );
    }
    if( quote_thread_size < quote_ports.size() )
    {
        throw std::logic_error( "too little threads for quote ports" );
    }
}


multicast_communication::market_data_receiver::~market_data_receiver(void)
{
}

void multicast_communication::market_data_receiver::start()
{
    for( int i = 0; i < quote_thread_size_; ++i )
    {
        quote_threads_.create_thread(
            boost::bind( &multicast_communication::market_data_receiver::proc, this ) );
    }
}

void multicast_communication::market_data_receiver::quote_proc(const std::string& address,
                                                               unsigned short port)
{
    boost::asio::io_service service;
    udp_listener listener(service, address, port, [this](const std::string& str)
    {
        std::istringstream input( str );
        quote_message_ptr msg;
        if( input.get() != 0x1 )
        {
            std::cout << "bad block" << std::endl;
            return;
        }

        do 
        {
            msg.reset( new quote_message() );
            input >> *( msg );
            processor_.new_quote( msg );

            // TODO?  skipping until 'US'

        } while ( input && input.get() == 0x1F );

        if( input.get() != 0x3 )
        {
            std::cout << "bad block" << std::endl;
            return;
        }
    });

}
