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

void multicast_communication::market_data_receiver::start()
{
    int j = 0;
    for( int i = 0; i < quote_thread_size_; ++i, ++j )
    {
        if ( j == quote_ports_.size() )
        {
            j = 0;
        }
        threads_.create_thread(
            boost::bind( &multicast_communication::market_data_receiver::quote_proc, this,
            quote_ports_[j].first, quote_ports_[j].second ) );
    }

    j = 0;
    for( int i = 0; i < trade_thread_size_; ++i, ++j )
    {
        if ( j == trade_ports_.size() )
        {
            j = 0;
        }
        threads_.create_thread(
            boost::bind( &multicast_communication::market_data_receiver::trade_proc, this,
            trade_ports_[j].first, trade_ports_[j].second ) );
    }
}

void multicast_communication::market_data_receiver::quote_proc(const std::string& address,
                                                               unsigned short port)
{
    udp_listener listener(service_, address, port, [&](const std::string& str)
    {
        quote_message_ptr_list msgs;
        if ( !quote_message::parse_block( str, msgs ) )
        {
            std::cout << "bad block from " << address << ":" << port << std::endl;
        }
        for( auto msg: msgs)
        {
            processor_.new_quote( msg );
        }
    });
    service_.run();
}

void multicast_communication::market_data_receiver::trade_proc(const std::string& address,
                                                               unsigned short port)
{
    udp_listener listener(service_, address, port, [&](const std::string& str)
    {
        trade_message_ptr_list msgs;
        if ( !trade_message::parse_block( str, msgs ) )
        {
            std::cout << "bad block from " << address << ":" << port << std::endl;
        }
        for( auto msg: msgs)
        {
            processor_.new_trade( msg );
        }
    });
    service_.run();
}

void multicast_communication::market_data_receiver::stop()
{
    service_.stop();
    threads_.join_all();
}
