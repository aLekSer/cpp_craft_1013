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
    start();
}


multicast_communication::market_data_receiver::~market_data_receiver(void)
{
    stop();
}

void service_run( std::shared_ptr< boost::asio::io_service > service)
{
    service->run();
}

void multicast_communication::market_data_receiver::start()
{
    for( auto& port: quote_ports_ )
    {
        service_ptr service ( new boost::asio::io_service() );
        udp_listener_ptr listener(
            new udp_listener( *service, port.first, port.second, [this](const std::string& block)
                {
                    threads_.create_thread( boost::bind(
                        &multicast_communication::market_data_receiver::quote_handler, this, block ) );
                } ) );
        quote_udp_listeners_.push_back( std::make_pair( service, listener) );
    }
    
    for( size_t i = 0, j = 0; i < quote_thread_size_; ++i, ++j )
    {
        if ( j == quote_ports_.size() )
        {
            j = 0;
        }
        threads_.create_thread( boost::bind( service_run, quote_udp_listeners_[ j ].first ) );
    }

    for( auto& port: trade_ports_ )
    {
        service_ptr service ( new boost::asio::io_service() );
        udp_listener_ptr listener(
            new udp_listener( *service, port.first, port.second, [this](const std::string& block)
                {
                    threads_.create_thread( boost::bind(
                        &multicast_communication::market_data_receiver::trade_handler, this, block ) );
                } ) );
        trade_udp_listeners_.push_back( std::make_pair( service, listener) );
    }

    for( size_t i = 0, j = 0; i < trade_thread_size_;  ++i, ++j )
    {
        if ( j == trade_ports_.size() )
        {
            j = 0;
        }
        threads_.create_thread( boost::bind( service_run, trade_udp_listeners_[ j ].first ) );
    }
}

void multicast_communication::market_data_receiver::quote_handler( const std::string& block)
{
    quote_message_ptr_list msgs;
    quote_message::parse_block( block, msgs );
    for( auto& msg: msgs )
    {
        processor_.new_quote( msg );
    }
}

void multicast_communication::market_data_receiver::trade_handler( const std::string& block)
{
    trade_message_ptr_list msgs;
    trade_message::parse_block( block, msgs );
    for( auto& msg: msgs )
    {
        processor_.new_trade( msg );
    }
}

void multicast_communication::market_data_receiver::stop()
{
    for ( auto& service_upd_lister_pair: quote_udp_listeners_ )
    {
        service_upd_lister_pair.first->stop();
    }

    for ( auto& service_upd_lister_pair: trade_udp_listeners_ )
    {
        service_upd_lister_pair.first->stop();
    }

    threads_.join_all();
}
