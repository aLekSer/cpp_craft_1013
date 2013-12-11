#ifndef _MINUTE_CALCULATOR_MINUTE_CALCULATOR_H_
#define _MINUTE_CALCULATOR_MINUTE_CALCULATOR_H_

#include <iostream>
#include <vector>
#include <map>

#include <boost/cstdint.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>

#include "thread_safe_queue.h"
#include "trade_message.h"
#include "quote_message.h"

using boost::uint32_t;

namespace minute_calculator
{
    using namespace common;
    struct minute_datafeed
    {
        minute_datafeed();
        uint32_t minute_;
        std::string stock_name_;
        double open_price_;
        double high_price_;
        double low_price_;
        double close_price_;
        double volume_;
        double bid_;
        double ask_;
        friend std::ostream& operator<<(std::ostream& os, minute_datafeed const&d);
    };

    typedef boost::shared_ptr<minute_datafeed> minute_datafeed_ptr; 
    typedef std::map<std::string, std::vector< minute_datafeed_ptr> > datafeeds_type;

    class calculator
    {
        typedef std::map<std::string, minute_datafeed_ptr> stock_df_;
        typedef boost::function<void (minute_datafeed_ptr, minute_datafeed_ptr)> merge_function;
        public: 
        calculator(thread_safe_queue< trade_message_ptr >& trades, 
                thread_safe_queue< quote_message_ptr>& quotes);
        ~calculator();
        std::map<std::string, std::vector< minute_datafeed_ptr> > get_minute_datafeed();
        void stop();
        void start();
        void process_trades();
        void process_quotes();
        void next_minute(stock_df_&
                , boost::uint32_t&
                , boost::uint32_t const, 
                merge_function);
        static boost::uint32_t minute_since_1900();
        static boost::uint32_t minute_from_timestamp(std::string const&);
        private:
        thread_safe_queue< trade_message_ptr >& trades_;
        boost::thread_group t_group_; 
        thread_safe_queue< quote_message_ptr >& quotes_;
        datafeeds_type datafeeds_;
        stock_df_ curr_t_;
        stock_df_ curr_q_;
        boost::uint32_t minute_t_;
        boost::uint32_t minute_q_;
        bool stop_;
        static void merge_trade_df(minute_datafeed_ptr, minute_datafeed_ptr);
        static void merge_quote_df(minute_datafeed_ptr, minute_datafeed_ptr);
        mutable boost::mutex protector_t_;
        mutable boost::mutex protector_q_;
        mutable boost::mutex protector_datafeeds_; 
    };

}
#endif
