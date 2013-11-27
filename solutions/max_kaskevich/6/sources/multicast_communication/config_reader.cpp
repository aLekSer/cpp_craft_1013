#include "config_reader.h"


bool multicast_communication::config_reader::read( std::istream& config )
{
    config >> trade_thread_size;
    config >> quote_thread_size;
    size_t trade_port_size = 0, quote_port_size = 0;
    config >> trade_port_size;

    std::string address;
    unsigned short port;
    while( trade_port_size-- )
    {
        config >> address >> port;
        trade_ports.push_back( std::make_pair( address, port ) );
    }
    config >> quote_port_size;
    while( quote_port_size-- )
    {
        config >> address >> port;
        quote_ports.push_back( std::make_pair( address, port ) );
    }
    return config;
}

multicast_communication::receiver_ptr multicast_communication::config_reader::generate_receiver()
{
    if(!processor)
    {
        return nullptr;
    }
    return receiver_ptr( new market_data_receiver( trade_thread_size, quote_thread_size,
            trade_ports, quote_ports, *processor) );
}
