#pragma once 
#ifndef _MULTICAST_COMMUNICATION_DATA_RECEIVER_H_
#define _MULTICAST_COMMUNICATION_DATA_RECEIVER_H_


#include "market_data_processor.h"
#include "udp_listener.h"
#include <boost/thread.hpp>
#include <boost/asio.hpp>

namespace multicast_communication
{
    typedef std::shared_ptr< udp_listener > udp_listener_ptr;
    typedef std::shared_ptr< boost::asio::io_service > service_ptr;
    typedef std::vector< std::pair< std::string, unsigned short > > ports;

    class market_data_receiver
    {
    private:
        const size_t trade_thread_size_;
        const size_t quote_thread_size_;
        const ports& trade_ports_;
        const ports& quote_ports_;
        boost::asio::io_service quote_service_;
        boost::asio::io_service trade_service_;
        market_data_processor& processor_;
        std::vector< udp_listener_ptr > quote_udp_listeners_;
        std::vector< udp_listener_ptr > trade_udp_listeners_;

        boost::thread_group threads_;
        bool working_;

        void quote_handler( const std::string& block, unsigned short port);
        void trade_handler( const std::string& block, unsigned short port);
    public:
        explicit market_data_receiver( const size_t trade_thread_size, const size_t quote_thread_size,
            const ports& quote_ports, const ports& trade_ports, market_data_processor& processor );
        virtual ~market_data_receiver();

        void start();
        void stop();
    };
}

#endif // _MULTICAST_COMMUNICATION_DATA_RECEIVER_H_
