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
    processor_( processor ),
    working_( false )
{
    start();
}


multicast_communication::market_data_receiver::~market_data_receiver(void)
{
    stop();
}

void multicast_communication::market_data_receiver::start()
{
    if ( working_ )
    {
        return;
    }
    working_ = true;
    for( auto& port: quote_ports_ )
    {
        udp_listener_ptr listener(
            new udp_listener( quote_service_, port.first, port.second, [&](const std::string& block)
                {
                    threads_.create_thread( boost::bind(
                        &multicast_communication::market_data_receiver::quote_handler, this, block, port.second ) );
                } ) );
        quote_udp_listeners_.push_back( listener );
    }
    
    for( size_t i = 0; i < quote_thread_size_; ++i )
    {
        threads_.create_thread( boost::bind( &boost::asio::io_service::run, &quote_service_ ) );
    }

    for( auto& port: trade_ports_ )
    {
        udp_listener_ptr listener(
            new udp_listener( trade_service_, port.first, port.second, [&](const std::string& block)
                {
                    threads_.create_thread( boost::bind(
                        &multicast_communication::market_data_receiver::trade_handler, this, block, port.second ) );
                } ) );
        trade_udp_listeners_.push_back( listener );
    }

    for( size_t i = 0; i < trade_thread_size_;  ++i )
    {
        threads_.create_thread( boost::bind( &boost::asio::io_service::run, &trade_service_ ) );
    }
}

void multicast_communication::market_data_receiver::quote_handler( const std::string& block,
                                                                   unsigned short port )
{
    quote_message_ptr_list msgs;
    if ( !parse_block( block, msgs ) )
    {
        std::cout << "market_data_receiver: bad block from " << port << std::endl;
    }
    for( auto& msg: msgs )
    {
        processor_.new_quote( msg );
    }
}

void multicast_communication::market_data_receiver::trade_handler( const std::string& block,
                                                                   unsigned short port )
{
    trade_message_ptr_list msgs;
    if ( !parse_block( block, msgs ) )
    {
        std::cout << "market_data_receiver: bad block from " << port << std::endl;
    }
    for( auto& msg: msgs )
    {
        processor_.new_trade( msg );
    }
}

void multicast_communication::market_data_receiver::stop()
{
    if ( !working_ )
    {
        return;
    }
    trade_service_.stop();
    quote_service_.stop();
    threads_.join_all();
    working_ = false;
}
