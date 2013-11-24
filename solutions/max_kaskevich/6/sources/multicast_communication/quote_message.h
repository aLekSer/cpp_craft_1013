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
    class quote_message;
    typedef boost::shared_ptr< quote_message > quote_message_ptr;
     typedef std::list< quote_message_ptr > quote_message_ptr_list;
	class quote_message
	{
    public:
        enum quote_type
        {
            ANOTHER,
            LONG_QUOTE,
            SHORT_QUOTE,
        };
        class header_type
        {
            quote_type type_;
        public:
            static const size_t size = 24;
            header_type() :
                type_(ANOTHER)
            {}

            friend std::istream& operator>>( std::istream& input, quote_message::header_type& header);

            quote_type type()
            {
                return type_;
            }
        };

    private:
        std::string security_symbol_;
        double bid_price_;
        double bid_volume_;
        double offer_price_;
        double offer_volume_;
        header_type header;

	public:

        quote_message() :
            security_symbol_(""),
            bid_price_(0.0),
            bid_volume_(0.0),
            offer_price_(0.0),
            offer_volume_(0.0),
            header()
        {        
        }

		std::string security_symbol() const;
		double bid_price() const;
		double bid_volume() const;
		double offer_price() const;
		double offer_volume() const;

        friend std::istream& operator>>( std::istream& input, quote_message& msg_ptr );

        quote_type type()
        {
            header.type();
        }

        void read_short( std::istream& input );
        void read_long( std::istream& input );
        
        static bool parse_block(const std::string& block, quote_message_ptr_list& msgs);
	};

	

    std::istream& operator>>( std::istream& input, quote_message::header_type& header );
    std::istream& operator>>( std::istream& input, quote_message& msg );
    std::ostream& operator<<( std::ostream& output, quote_message& msg );

}

#endif // _MULTICAST_COMMUNICATION_QUOTE_MESSAGE_H_
