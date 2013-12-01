#pragma once 
#ifndef  _MINUTE_MARKET_MINUTE_MARKET_DATA_H_
#define  _MINUTE_MARKET_MINUTE_MARKET_DATA_H_

#include "market_data_processor.h"
#include "minute_market_calculator.h"

using namespace multicast_communication;

namespace minute_market
{

    class minute_market_data : public market_data_processor
    {
        friend class minute_market_data_test_helper;

        const std::string dir_path_;
        minute_market_calculator calculator_;
    public:
        explicit minute_market_data( const std::string& dir_path,
                                     minute_market_calculator& calculator );
    private:
        // market_data_processor implementation
        void new_trade( const trade_message_ptr& msg );
        void new_quote( const quote_message_ptr& msg );

        void on_minute_data_feed( const minute_datafeed& data );

    };
}

#endif // _MINUTE_MARKET_MINUTE_MARKET_DATA_H_