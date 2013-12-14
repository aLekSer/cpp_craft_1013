#pragma once 
#ifndef  _MINUTE_MARKET_MINUTE_MARKET_DATA_H_
#define  _MINUTE_MARKET_MINUTE_MARKET_DATA_H_

#include "market_data_processor.h"
#include "minute_market_calculator.h"
#include <map>
#include <boost/thread.hpp>
#include <fstream>
#include "task_engine.h"

using namespace multicast_communication;

namespace minute_market
{

    class minute_market_data : public market_data_processor
    {
        friend class minute_market_data_test_helper;
        task_engine engine_;
        const std::string dir_path_;
        minute_market_calculator calculator_;

        std::map< const std::string, std::ofstream > outputs_;
        boost::mutex output_mtx_;
        bool stopped_;
    public:
        explicit minute_market_data( const std::string& dir_path );
        ~minute_market_data();

        void stop();
    private:
        // market_data_processor implementation
        void new_trade( const trade_message_ptr& msg );
        void new_quote( const quote_message_ptr& msg );

        void on_minute_data_feed( const minute_datafeed& data );

    };
}

#endif // _MINUTE_MARKET_MINUTE_MARKET_DATA_H_