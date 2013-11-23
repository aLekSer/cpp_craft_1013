#pragma once
#include "market_data_processor.h"
#include "thread_safe_queue.h"
#include <boost/thread.hpp>

namespace multicast_communication
{

    class async_writer : public market_data_processor
    {
        std::ostream& output_;
        thread_safe_queue< quote_message_ptr > queue_;
        boost::thread thread_; 
    public:
        explicit async_writer( std::ostream& output );
        ~async_writer(void);

        void join();
    private:
        void proc();

        // market_data_processor implementing 
        virtual void new_trade( const trade_message_ptr& msg );
        virtual void new_quote( const quote_message_ptr& msg );
    };

}
