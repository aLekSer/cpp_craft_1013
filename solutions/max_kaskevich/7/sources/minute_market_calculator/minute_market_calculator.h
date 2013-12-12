#pragma once 
#ifndef  _MINUTE_MARKET_DATA_MINUTE_MARKET_CALCULATOR_H_
#define  _MINUTE_MARKET_DATA_MINUTE_MARKET_CALCULATOR_H_
#include <stdint.h>
#include <unordered_map>
#include "market_data_processor.h"
#include <boost/thread.hpp>



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
        double close_prise;
        double volume;
        double bid;
        double ask;

        minute_datafeed() :
            minute( 0u ),
            //stock_name(),
            open_prise( 0.0 ),
            high_prise( 0.0 ),
            low_price ( 0.0 ),
            close_prise( 0.0 ),
            volume( 0.0 ),
            bid( 0.0 ),
            ask( 0.0 )
        {
            memset(&stock_name[0], 0, sizeof(stock_name));
        }
    };

    std::ostream& operator<<( std::ostream& output, const minute_datafeed& mdf );

    class minute_market_calculator
    {
        struct safe_minute_datafeed
        {
            boost::mutex mtx_;
            minute_datafeed mdf;
            bool have_trades;
            bool have_quotes;
        };
        typedef std::shared_ptr< safe_minute_datafeed > safe_minute_datafeed_ptr;
        typedef std::unordered_map< std::string, safe_minute_datafeed_ptr > stock_name_map_type;
        stock_name_map_type stock_name_map_;
        typedef std::function< void ( const minute_datafeed& ) > callback_type;
        callback_type callback_;

        boost::shared_mutex map_mtx_;

    public:
        static const size_t time_interval = 60u;

        explicit minute_market_calculator( callback_type callback );
        ~minute_market_calculator();

        void new_trade( const trade_message_ptr& );
        void new_quote( const quote_message_ptr& );

        void dump();

        template< class MsgType >
        void new_msg( MsgType msg, std::function< void( minute_datafeed&, MsgType ) > msg_handler,
            std::function< void( minute_datafeed&, MsgType ) > first_msg_handler,
            std::function< bool( safe_minute_datafeed& ) > check_set_first_msg_for )
        {
            stock_name_map_type::iterator it;
            safe_minute_datafeed_ptr smdf;
            {
                boost::shared_lock< boost::shared_mutex > read_lock( map_mtx_ );
                it = stock_name_map_.find( msg->security_symbol() );
                if( it != stock_name_map_.end() )
                {
                    smdf = it->second;
                }
            }

            if( !smdf )
            {
                boost::unique_lock< boost::shared_mutex > write_lock( map_mtx_ );
                auto res = stock_name_map_.emplace( msg->security_symbol(), safe_minute_datafeed_ptr() );
                it = res.first;
                if ( res.second )
                {
                    it->second.reset( new safe_minute_datafeed() );
                }
                smdf = it->second;
            }

            {
                boost::shared_lock< boost::shared_mutex > read_lock( map_mtx_ );

                minute_datafeed& mdf =  smdf->mdf;

                if ( msg->time() >= mdf.minute + minute_market_calculator::time_interval )
                {
                    boost::mutex::scoped_lock lock( smdf->mtx_ );                
                    if ( mdf.minute )
                    {
                        callback_( mdf );
                    }
                    mdf = minute_datafeed();
                    check_set_first_msg_for( *smdf );
                    first_msg_handler( mdf, msg );
                    mdf.minute = msg->time();
                    size_t stock_name_legnth = msg->security_symbol().length();
                    std::memcpy( mdf.stock_name, msg->security_symbol().c_str(), stock_name_legnth );
                }
                else if ( msg->time() >= mdf.minute )
                {
                   boost::mutex::scoped_lock lock( smdf->mtx_ );
                   if ( check_set_first_msg_for( *smdf ) )
                   {
                       first_msg_handler( mdf, msg );
                   }
                   else
                   {
                       msg_handler( mdf, msg );
                   }
                }
            }
        }
    };
}

#endif // _MINUTE_MARKET_DATA_MINUTE_MARKET_CALCULATOR_H_