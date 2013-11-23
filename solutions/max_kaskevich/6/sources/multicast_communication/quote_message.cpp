#include "quote_message.h"
#include <math.h>
#include <boost/algorithm/string.hpp>

using namespace std;


static const std::map< const char, double > denominator_map = boost::assign::map_list_of
     // FRACTIONAL
    ( '3' , 8.0 ) ( '4' , 16.0 ) ( '5' , 32.0 ) ( '6' , 64.0 ) ( '7' , 128.0 ) ( '8' , 256.0 )
     // DECIMAL
    ( 'A' , 10.0 ) ( 'B' , pow( 10.0, 2 ) ) ( 'C' , pow( 10.0, 3 ) ) ( 'D' , pow( 10.0, 4 ) )
    ( 'E' , pow( 10.0, 5 ) ) ( 'F' , pow( 10.0, 6 ) ) ( 'G' , pow( 10.0, 7 ) )
    ( 'H' , pow( 10.0, 8 ) )

    ('I' , 1.0 )
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

std::istream& multicast_communication::operator>>( std::istream& input, quote_message::header_type& header)
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

template< size_t size >
double read_numeric( std::istream& input )
{
    char buf[ size ];
    input.read( buf, size );
    return boost::lexical_cast< double >( buf, size );
}

template< size_t size >
std::string read_alphabetic( std::istream& input )
{
    char buf[ size ];
    input.read( buf, size );
    return string( buf , size );
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

