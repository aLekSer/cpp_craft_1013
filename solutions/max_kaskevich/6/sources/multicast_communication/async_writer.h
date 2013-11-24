#pragma once
#include "market_data_processor.h"
#include "thread_safe_queue.h"
#include <boost/thread.hpp>

namespace multicast_communication
{

    class async_writer : public market_data_processor
    {
        std::ostream& output_;
        thread_safe_queue< quote_message_ptr > quote_queue_;
        thread_safe_queue< trade_message_ptr > trade_queue_;
        boost::thread quote_thread_; 
        boost::thread trade_thread_; 
        boost::mutex mtx_;
    public:
        explicit async_writer( std::ostream& output );
        ~async_writer(void);

        void join();
    private:
        void quote_proc();
        void trade_proc();

        // market_data_processor implementation
        virtual void new_trade( const trade_message_ptr& msg );
        virtual void new_quote( const quote_message_ptr& msg );
    };

}
