#pragma once
#ifndef _MULTICAST_COMMUNICATION_COMMON_MESSAGE_H_
#define _MULTICAST_COMMUNICATION_COMMON_MESSAGE_H_

#include <boost/shared_ptr.hpp>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/assign.hpp>

namespace multicast_communication
{

    static const std::map< const char, double > denominator_map = boost::assign::map_list_of
        // FRACTIONAL
        ( '3' , 8.0 ) ( '4' , 16.0 ) ( '5' , 32.0 ) ( '6' , 64.0 ) ( '7' , 128.0 ) ( '8' , 256.0 )
        // DECIMAL
        ( 'A' , 10.0 ) ( 'B' , pow( 10.0, 2 ) ) ( 'C' , pow( 10.0, 3 ) ) ( 'D' , pow( 10.0, 4 ) )
        ( 'E' , pow( 10.0, 5 ) ) ( 'F' , pow( 10.0, 6 ) ) ( 'G' , pow( 10.0, 7 ) )
        ( 'H' , pow( 10.0, 8 ) )

        ('I' , 1.0 ) ('0' , 1.0 )
        ;

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
        return std::string( buf , size );
    }

}

#endif // _MULTICAST_COMMUNICATION_COMMON_MESSAGE_H_
