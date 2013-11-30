#pragma once
#ifndef _MULTICAST_COMMUNICATION_CONFIG_READER_H_
#define _MULTICAST_COMMUNICATION_CONFIG_READER_H_

#include <iostream>
#include "market_data_receiver.h"

namespace multicast_communication
{
    typedef std::shared_ptr< market_data_receiver > receiver_ptr;
    class config_reader
    {
    public:
        size_t trade_thread_size;
        size_t quote_thread_size;
        ports trade_ports;
        ports quote_ports;
        market_data_processor* processor;

        bool read( std::istream& config );

        receiver_ptr generate_receiver();
    };
} // multicast_communication
#endif // _MULTICAST_COMMUNICATION_CONFIG_READER_H_
