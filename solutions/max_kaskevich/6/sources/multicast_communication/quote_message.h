#ifndef _MULTICAST_COMMUNICATION_QUOTE_MESSAGE_H_
#define _MULTICAST_COMMUNICATION_QUOTE_MESSAGE_H_

#include <boost/shared_ptr.hpp>
#include <iostream>
#include <boost/multiprecision/cpp_int.hpp>
#include <memory>
#include <boost/lexical_cast.hpp>
#include <boost/assign.hpp>

using namespace boost::multiprecision;

namespace multicast_communication
{

	class quote_message
	{
    protected:
        static const size_t size;

        std::string security_symbol_;
        double bid_price_;
        double bid_volume_;
        double offer_price_;
        double offer_volume_;

	public:

        static const std::map< const char, uint32_t > denominator_map;

        enum quote_type
        {
            ANOTHER,
            LONG_QUOTE,
            SHORT_QUOTE,
        };
        class header
        {
            quote_type type_;
        public:
            static const size_t size = 24;
            header() :
                type_(ANOTHER)
            {}

            friend std::istream& operator>>( std::istream& input, quote_message::header& header);

            quote_type type()
            {
                return type_;
            }
        };

        quote_message() :
            security_symbol_(""),
            bid_price_(0.0),
            bid_volume_(0.0),
            offer_price_(0.0),
            offer_volume_(0.0)
        {        
        }

		std::string security_symbol() const;
		double bid_price() const;
		double bid_volume() const;
		double offer_price() const;
		double offer_volume() const;

	};

	typedef boost::shared_ptr< quote_message > quote_message_ptr;


    template<class T>
    void read( T* msg, std::istream& input )
    {

        char buf[ msg->size - quote_message::header::size ];
        input.read( buf, msg->size );
        msg->security_symbol_.append( buf + msg->security_symbol_offset , msg->security_symbol_size );

        msg->bid_price_ = boost::lexical_cast< double >( buf  + msg->bid_price_offset, msg->bid_price_size );
        msg->bid_price_ = msg->bid_price_ /
            quote_message::denominator_map.at( buf[ msg->bid_denominator_offset ] );
        msg->bid_volume_ = boost::lexical_cast< uint32_t >( buf + msg->bid_volume_offset, msg->bid_volume_size );

        msg->offer_price_ = boost::lexical_cast< double >( buf + msg->offer_price_offset, msg->offer_price_size );
        msg->offer_price_ = msg->offer_price_ / 
            quote_message::denominator_map.at( buf[ msg->offer_denominator_offset ] );
        msg->offer_volume_ = boost::lexical_cast< uint32_t >( buf + msg->offer_volume_offset, msg->offer_volume_size );

    }

    class long_quote_message : public quote_message
    {
        friend void read< long_quote_message >( long_quote_message* msg, std::istream& input );
    public:
        long_quote_message( std::istream& input )
        {
            read( this, input );
        }

        static const size_t size = 102;

        static const size_t security_symbol_offset = 0;
        static const size_t security_symbol_size = 3;

        static const size_t bid_denominator_offset = 25;
        static const size_t bid_denominator_size = 1;
        static const size_t bid_price_offset = 26;
        static const size_t bid_price_size = 8;
        static const size_t bid_volume_offset = 34;
        static const size_t bid_volume_size = 7;

        static const size_t offer_denominator_offset = 41;
        static const size_t offer_denominator_size = 1;
        static const size_t offer_price_offset = 42;
        static const size_t offer_price_size = 8;
        static const size_t offer_volume_offset = 50;
        static const size_t offer_volume_size = 7;
    };

    class short_quote_message : public quote_message
    {
         friend void read< short_quote_message >( short_quote_message* msg, std::istream& input );
    public:
        short_quote_message( std::istream& input )
        {
            read( this, input );
        }

        static const size_t size = 58;

        static const size_t security_symbol_offset = 0;
        static const size_t security_symbol_size = 3;

        static const size_t bid_denominator_offset = 6;
        static const size_t bid_denominator_size = 1;
        static const size_t bid_price_offset = 7;
        static const size_t bid_price_size = 8;
        static const size_t bid_volume_offset = 15;
        static const size_t bid_volume_size = 3;

        static const size_t offer_denominator_offset = 19;
        static const size_t offer_denominator_size = 1;
        static const size_t offer_price_offset = 20;
        static const size_t offer_price_size = 8;
        static const size_t offer_volume_offset = 28;
        static const size_t offer_volume_size = 3;
    };


    std::istream& operator>>( std::istream& input, quote_message::header& header );
    std::istream& operator>>( std::istream& input, quote_message_ptr& msg_ptr );

}

#endif // _MULTICAST_COMMUNICATION_QUOTE_MESSAGE_H_
