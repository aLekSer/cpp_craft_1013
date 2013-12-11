#include <vector>
#include <string>
#include <stdexcept>
#include <functional>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/noncopyable.hpp>

#include "market_data_receiver.h"
#include "market_data_processor.h"

namespace multicast_communication
{
    using namespace common;
    void market_data_receiver::receive(udp_listener::message const& m)
    { 
        if(m.type_ == udp_listener::message::TRADE)
        {
            trade_queue_.push( m.data );
        }
        else if(m.type_ == udp_listener::message::QUOTE)
        {
            quote_queue_.push( m.data );
        }
    }

    market_data_receiver::market_data_receiver(
        size_t const trade_thread_size,
        size_t const quote_thread_size,
        std::vector<udp_listener::endpoint_addr> const& trade_ports,
        std::vector<udp_listener::endpoint_addr> const& quote_ports,
        market_data_processor& processor )
        : trade_thread_size_(trade_thread_size),
        quote_thread_size_(quote_thread_size),
        processor_( processor )
    { 
        for(std::vector<udp_listener::endpoint_addr>::const_iterator it = trade_ports.begin();
            it != trade_ports.end(); ++it)
        {
            udp_listener::udp_listener_ptr c_ptr( 
                    boost::make_shared<udp_listener>(
                        *it, 
                        boost::bind(&market_data_receiver::receive, this, _1), 
                        udp_listener::message::TRADE, 
                        io_service_ ) );

            listeners_.push_back(c_ptr);
            c_ptr->socket_reload_();
            c_ptr->register_listen_();
        }
        for(std::vector<udp_listener::endpoint_addr>::const_iterator it = quote_ports.begin();
            it != quote_ports.end(); ++it)
        {
            udp_listener::udp_listener_ptr c_ptr( 
                    boost::make_shared<udp_listener>(
                        *it, 
                        boost::bind(&market_data_receiver::receive, this, _1), 
                        udp_listener::message::QUOTE, 
                        io_service_ ) );

            listeners_.push_back(c_ptr);
            c_ptr->socket_reload_();
            c_ptr->register_listen_();
        }
    }

    market_data_receiver::~market_data_receiver()
    {
        try { this->stop(); } catch(...){}
    } 

    void market_data_receiver::start()
    {
        boost::thread service_thread( boost::bind( &boost::asio::io_service::run, boost::ref( io_service_ )) );

        for(size_t i = 0; i < trade_thread_size_; ++i)
            t_group_.create_thread(boost::bind(&market_data_receiver::process_trades_, this));

        for(size_t i = 0; i < quote_thread_size_; ++i)
            t_group_.create_thread(boost::bind(&market_data_receiver::process_quotes_, this));

        stop_ = false;
    }

    void market_data_receiver::stop()
    {
        stop_ = true;
        io_service_.stop();
        trade_queue_.stop();
        quote_queue_.stop();
        t_group_.join_all();
    } 

    void market_data_receiver::process_trades_()
    {
        while(true)
        {
            message_type s;
            trade_queue_.wait_and_pop( s );
            if(stop_)
            {
                break;
            }
            std::vector< message_type > trades;            
            market_data_processor::split_block( s, trades );
            for(std::vector<message_type>::iterator it = trades.begin(); it != trades.end(); ++it)
            {
                trade_message_ptr t = boost::make_shared<trade_message>(*it);
                processor_.new_trade( t ); 
            }
        }
    }

    void market_data_receiver::process_quotes_()
    {
        while(true)
        {
            message_type s;
            quote_queue_.wait_and_pop( s );
            if(stop_)
            {
                break;
            }
            std::vector< message_type > quotes;
            market_data_processor::split_block( s, quotes );
            for(std::vector< message_type >::iterator it = quotes.begin(); it != quotes.end(); ++it)
            {
                quote_message_ptr q = boost::make_shared<quote_message>(*it);
                processor_.new_quote( q );
            }
        }
    } 

    void market_data_receiver::join()
    { 
        t_group_.join_all();
    }

    size_t market_data_receiver::trades_size() const
    {
        return trade_queue_.size();
    }
    size_t market_data_receiver::quotes_size() const
    {
        return quote_queue_.size();
    }
}
