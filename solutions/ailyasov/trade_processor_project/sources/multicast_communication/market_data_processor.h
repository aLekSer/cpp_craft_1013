#ifndef _MULTICAST_COMMUNICATION_MARKET_DATA_PROCESSOR_H_
#define _MULTICAST_COMMUNICATION_MARKET_DATA_PROCESSOR_H_

#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "trade_message.h"
#include "thread_safe_queue.h" 
#include "quote_message.h"

namespace multicast_communication
{
    using namespace common;

    class market_data_processor : protected virtual boost::noncopyable
    {
        public:
            explicit market_data_processor() { }
            virtual ~market_data_processor(){}
            virtual thread_safe_queue< trade_message_ptr >& trades();
            virtual thread_safe_queue< quote_message_ptr >& quotes();
            void save(std::string const&);
            typedef std::map<char, int> denom_type;
        protected:
            friend class market_data_receiver;
            static denom_type denom_map; 
            static int get_denom(char); 
            virtual void new_trade( trade_message_ptr const );
            virtual void new_quote( quote_message_ptr const ); 

            thread_safe_queue< trade_message_ptr > trades_;
            thread_safe_queue< quote_message_ptr > quotes_;

            static void split_block(message_type const& m, std::vector<message_type>& v);

            template <typename T>
                static void parse_quote( quote_message_ptr ptr )
                {
                    BOOST_ASSERT( ptr != 0 );
                    if(!is_valid_type<short_quote_message>(ptr->raw_) && !is_valid_type<long_quote_message>(ptr->raw_) )
                        throw std::logic_error("Invalid quote type");
                    message_type const& raw = ptr->raw_;
                    ptr->category_ = raw[0];
                    ptr->type_ = raw[1];
                    ptr->timestamp_ = extract<T, std::string>(raw, T::TIMESTAMP);
                    ptr->security_symbol_ = extract<T, std::string>(raw, T::SECURITY_SYMBOL);                    
                    boost::algorithm::trim(ptr->security_symbol_);
                    ptr->bid_price_demoninator_indicator_ =  extract<T, char>(raw, T::BID_PRICE_DENOMINATOR_INDICATOR);
                    ptr->bid_price_ = extract<T, double>(raw, T::BID_PRICE) / get_denom(ptr->bid_price_demoninator_indicator_);
                    ptr->bid_size_ = extract<T, double>(raw, T::BID_SIZE);
                    ptr->offer_price_denominator_indicator_ = extract<T, char>(raw, T::OFFER_PRICE_DENOMINATOR_INDICATOR);
                    ptr->offer_price_ = extract<T, double>(raw, T::OFFER_PRICE) / get_denom(ptr->offer_price_denominator_indicator_);
                    ptr->offer_size_ = extract<T, double>(raw, T::OFFER_SIZE);
                }

            template <typename T>
                static void parse_trade( trade_message_ptr ptr )
                {
                    BOOST_ASSERT( ptr != 0 );
                    if(!is_valid_type<short_trade_message>(ptr->raw_) && !is_valid_type<long_trade_message>(ptr->raw_) )
                        throw std::logic_error("Invalid trade type");
                    message_type const& raw = ptr->raw_;
                    ptr->category_ = raw[0];
                    ptr->type_ = raw[1];
                    ptr->timestamp_ = extract<T, std::string>(raw, T::TIMESTAMP);
                    ptr->security_symbol_ = extract<T, std::string>(raw, T::SECURITY_SYMBOL);
                    boost::algorithm::trim(ptr->security_symbol_);
                    ptr->price_denom_code_ = extract<T, char>(raw, T::PRICE_DENOMINATOR_INDICATOR);
                    ptr->price_ = extract<T, double>(raw, T::TRADE_PRICE) / get_denom(ptr->price_denom_code_);
                    ptr->volume_ = extract<T, double>(raw, T::TRADE_VOLUME);
                }

            template <typename T>
                static bool is_valid_type(message_type const& m)
                {
                    for(size_t i = 0; i < T::RESTRICTION_SIZE; ++i)
                    {
                        if( m[0] == T::RESTRICTION_CATEGORY[i] && m[1] == T::RESTRICTION_TYPE[i] )
                            return true;
                    }
                    return false;
                }

            template <typename T>
                static message_type extract_field(message_type const& raw, int f)
                {
                    return message_type(raw.begin() + T::OFFSET[f], raw.begin() + T::OFFSET[f] + T::LENGTH[f]);
                }

            template <typename T, typename N>
                static N extract(message_type const& raw, int f)
                {
                    message_type m = extract_field<T>(raw, f);
                    return boost::lexical_cast<N>( std::string(m.begin(), m.begin() + m.size()) );
                }
    };

}

#endif // _MULTICAST_COMMUNICATION_MARKET_DATA_PROCESSOR_H_
