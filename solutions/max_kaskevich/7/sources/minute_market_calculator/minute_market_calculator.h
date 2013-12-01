#pragma once 
#ifndef  _MINUTE_MARKET_DATA_MINUTE_MARKET_CALCULATOR_H_
#define  _MINUTE_MARKET_DATA_MINUTE_MARKET_CALCULATOR_H_
#include <stdint.h>
#include <unordered_map>
#include "market_data_processor.h"
#include "thread_safe_queue.h"



using namespace multicast_communication;

namespace minute_market
{
    struct minute_datafeed
    {
        uint32_t minute; // time from 1900.01.01 00:00:00
        char stock_name[16]; // null-terminated
        double open_prise;
        double high_prise;
        double low_price;
        double close_prose;
        double volume;
        double bid;
        double ask;
    };


    class minute_market_calculator
    {
        typedef std::unordered_map< std::string, minute_datafeed > stock_name_map_type;
        stock_name_map_type stock_name_map_;
        typedef std::function< void ( const minute_datafeed& ) > callback_type;
        callback_type callback_;

        multicast_communication::thread_safe_queue< thre
    public:
        minute_market_calculator( callback_type callback );

        void new_trade( const trade_message_ptr& );
        void new_quote( const quote_message_ptr& );
    };
}

#endif // _MINUTE_MARKET_DATA_MINUTE_MARKET_CALCULATOR_H_