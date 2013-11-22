#include "quote_message.h"
#include <math.h>

using namespace std;

const std::map< const char, double > multicast_communication::quote_message::denominator_map
    = boost::assign::map_list_of
     // FRACTIONAL
    ('3' , 8.0) ('4' , 16.0) ('5' , 32.0) ('6' , 64.0) ('7' , 128.0) ('8' , 256.0)
     // DECIMAL
    ('A' , 10.0) ('B' , pow( 10.0, 2 ) ) ('C' , pow( 10.0, 3 ) ) ('D' , pow( 10.0, 4 ) )
    ('E' , ( 10.0, 5) ) ('F' , pow( 10.0, 6 ) ) ('G' , pow( 10.0, 7 ) )
    ('H' , pow( 10.0, 8) )

    ('I' , 1.0)
    ;

std::string multicast_communication::quote_message::security_symbol() const
{
	return security_symbol_;
}

double multicast_communication::quote_message::bid_price() const
{
	return bid_price_;
}

double multicast_communication::quote_message::bid_volume() const
{
	return bid_volume_;
}

double multicast_communication::quote_message::offer_price() const
{
	return offer_price_;
}

double multicast_communication::quote_message::offer_volume() const
{
	return offer_volume_;
}

std::istream& multicast_communication::operator>>( std::istream& input, quote_message::header& header)
{
    input.ignore();
    switch ( input.get() )
    {
    case 'B':
        header.type_ = quote_message::LONG_QUOTE;
        break;
    case 'D':
        header.type_ = quote_message::SHORT_QUOTE;
        break;
    default:
        break;
    }
    input.ignore(22);
    return input;
}

std::istream& multicast_communication::operator>>( std::istream& input, quote_message_ptr& msg_ptr)
{
    using namespace multicast_communication;
    quote_message::header h;
    input >> h;
    switch ( h.type() )
    {
    case quote_message::SHORT_QUOTE:
        msg_ptr.reset( new short_quote_message( input ) );
        break;
    case quote_message::LONG_QUOTE:
        msg_ptr.reset( new long_quote_message( input ) );
        break;
    }
    
    return input;
}
