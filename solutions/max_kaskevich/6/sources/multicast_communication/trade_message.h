#ifndef _MULTICAST_COMMUNICATION_TRADE_MESSAGE_H_
#define _MULTICAST_COMMUNICATION_TRADE_MESSAGE_H_

#include <boost/shared_ptr.hpp>
#include <iostream>
#include <list>
#include "common_message.h"

namespace multicast_communication
{
    class trade_message;
    typedef boost::shared_ptr< trade_message > trade_message_ptr;
    typedef std::list< trade_message_ptr > trade_message_ptr_list;

    class trade_message
    {
    public:
        enum trade_type
        {
            ANOTHER,
            LONG_TRADE,
            SHORT_TRADE,
        };
        class header_type
        {
            trade_type type_;
            uint32_t time_;

            friend std::istream& operator>>( std::istream& input, trade_message::header_type& header);
        public:
            static const size_t size = 24;
            header_type() :
                type_( ANOTHER ),
                time_( 0u )
            {}


            trade_type type()
            {
                return type_;
            }
            uint32_t time()
            {
                return time_;
            }
        };

        trade_message() :
            security_symbol_(),
            price_(0.0),
            volume_(0.0)
        {}

        trade_type type()
        {
            return header.type();
        }

        uint32_t time()
        {
            return header.time();
        }

        std::string security_symbol() const;
        double price() const;
        double volume() const;

    private:
        std::string security_symbol_;
        double price_;
        double volume_;
        header_type header;

        void read_short( std::istream& input );
        void read_long( std::istream& input );

        friend std::istream& operator>>( std::istream& input, trade_message& msg_ptr );

    };


    std::istream& operator>>( std::istream& input, trade_message::header_type& header );
    std::istream& operator>>( std::istream& input, trade_message& msg );
    std::ostream& operator<<( std::ostream& output, trade_message& msg );
}

#endif // _MULTICAST_COMMUNICATION_TRADE_MESSAGE_H_
