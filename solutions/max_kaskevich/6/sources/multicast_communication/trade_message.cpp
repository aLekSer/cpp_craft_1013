#include "trade_message.h"
#include "common_message.h"
#include <iomanip>
#include <boost/algorithm/string.hpp>
#include <boost/assign.hpp>

std::string multicast_communication::trade_message::security_symbol() const
{
    return security_symbol_;
}

double multicast_communication::trade_message::price() const
{
    return price_;
}

double multicast_communication::trade_message::volume() const
{
    return volume_;
}

std::istream& multicast_communication::operator>>( std::istream& input, trade_message::header_type& header)
{
    char category, type;
    category = input.get();
    type = input.get();
    if(type == 'I')
    {
        switch ( category )
        {
        case 'E':
        case 'L':
            header.type_ = trade_message::SHORT_TRADE;
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
            header.type_ = trade_message::LONG_TRADE;
            break;
        default:
            header.type_ = trade_message::ANOTHER;
            break;
        }
    }
    input.ignore( 22 ); // unnecessary data
    return input;
}

std::istream& multicast_communication::operator>>( std::istream& input, trade_message& msg )
{
    input >> msg.header;
    switch ( msg.header.type() )
    {
    case trade_message::SHORT_TRADE:
        msg.read_short( input );
        break;
    case trade_message::LONG_TRADE:
        msg.read_long( input );
        break;
    }
    return input;
}


void multicast_communication::trade_message::read_short( std::istream& input )
{
    security_symbol_ = read_alphabetic< 3 >( input );
    boost::trim( security_symbol_ );

    input.ignore( 1 ); // unnecessary data

    volume_ = read_numeric< 4 >( input );
    double denominator = denominator_map.at( input.get() );
    price_ = read_numeric< 8 >( input ) / denominator;

    input.ignore( 3 ); // unnecessary data
}

void multicast_communication::trade_message::read_long( std::istream& input )
{
    security_symbol_ = read_alphabetic< 11 >( input );
    boost::trim( security_symbol_ );

    input.ignore( 21 ); // unnecessary data

    double denominator = denominator_map.at( input.get() );
    price_ = read_numeric< 12 >( input ) / denominator;
    volume_ = read_numeric< 9 >( input );

    input.ignore( 4 ); // unnecessary data
}

std::ostream& multicast_communication::operator<<( std::ostream& output, trade_message& msg )
{
    output  << std::fixed << "T " << msg.security_symbol() << " "
        << std::setprecision( 2 )  << msg.price() << " "
        << std::setprecision( 1 ) << msg.volume() << std::endl;
    return output;
}

bool multicast_communication::trade_message::parse_block(const std::string& block,
                                                         trade_message_ptr_list& msgs)
{
    trade_message_ptr_list list;
    std::istringstream input( block );
    trade_message_ptr msg;
    if( input.get() != 0x1 )
    {
        return false;
    }
    char c;
    do 
    {
        msg.reset( new trade_message() );
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
