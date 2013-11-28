#include "quote_message.h"
#include "common_message.h"
#include <iomanip>
#include <boost/algorithm/string.hpp>

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

std::istream& multicast_communication::operator>>( std::istream& input, quote_message::header_type& header)
{
    char category, type;
    category = input.get();
    type = input.get();
    if(type == 'D')
    {
        switch ( category )
        {
        case 'E':
        case 'L':
            header.type_ = quote_message::SHORT_QUOTE;
            break;
        default:
            break;
        }
    }
    else if (type == 'B')
    {
        switch ( category )
        {
        case 'B':
        case 'E':
        case 'L':
            header.type_ = quote_message::LONG_QUOTE;
            break;
        default:
            header.type_ = quote_message::ANOTHER;
            break;
        }
    }
    input.ignore( 22 ); // unnecessary data
    return input;
}

std::istream& multicast_communication::operator>>( std::istream& input, quote_message& msg )
{
    input >> msg.header;
    switch ( msg.header.type() )
    {
    case quote_message::SHORT_QUOTE:
        msg.read_short( input );
        break;
    case quote_message::LONG_QUOTE:
        msg.read_long( input );
        break;
    }
    return input;
}

void multicast_communication::quote_message::read_short( std::istream& input )
{
    security_symbol_ = read_alphabetic< 3 >( input );
    boost::trim( security_symbol_ );

    input.ignore( 3 ); // unnecessary data

    double denominator = denominator_map.at( input.get() );
    bid_price_ = read_numeric< 8 >( input ) / denominator;
    bid_volume_ = read_numeric< 3 >( input );

    input.ignore(); // unnecessary data

    denominator = denominator_map.at( input.get() );
    offer_price_ = read_numeric< 8 >( input ) / denominator;
    offer_volume_ = read_numeric< 3 >( input );

    input.ignore( 3 ); // unnecessary data
}

void multicast_communication::quote_message::read_long( std::istream& input )
{
    security_symbol_ = read_alphabetic< 11 >( input );
    boost::trim( security_symbol_ );

    input.ignore( 16 ); // unnecessary data

    double denominator = denominator_map.at( input.get() );
    bid_price_ = read_numeric< 12 >( input ) / denominator;
    bid_volume_ = read_numeric< 7 >( input );

    denominator = denominator_map.at( input.get() );
    offer_price_ = read_numeric< 12 >( input ) / denominator;
    offer_volume_ = read_numeric< 7 >( input );

    input.ignore( 11 ); // unnecessary data
}

std::ostream& multicast_communication::operator<<( std::ostream& output, quote_message& msg )
{
    output  << std::fixed << "Q " << msg.security_symbol() << " "
        << std::setprecision( 2 )  << msg.bid_price() << " "
        << std::setprecision( 1 ) << msg.bid_volume() << " "
        << std::setprecision( 2 ) << msg.offer_price() << " "
        << std::setprecision( 1 ) << msg.offer_volume() << std::endl;
    return output;
}

bool multicast_communication::quote_message::parse_block(const std::string& block,
                                                         quote_message_ptr_list& msgs)
{
    quote_message_ptr_list list;
    std::istringstream input( block );
    quote_message_ptr msg;
    if( input.get() != 0x1 )
    {
        return false;
    }
    char c;
    do 
    {
        msg.reset( new quote_message() );
        input >> *( msg );
        if (msg->type() != ANOTHER)
        {
            msgs.push_back( msg );
        }

        while (input >> c && c != 0x1F && c != 0x3 )
        {}

    } while ( input && c == 0x1F );

    if( c != 0x3 )
    {        
        return false;
    }
    return true;
}
