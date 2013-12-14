#include "test_registrator.h"

#include <quote_message.h>
#include <fstream>

static const double eps = 0.0000001;

namespace multicast_communication
{
    static size_t max_block_size = 1000u;
    namespace quote_tests_
    {

        bool get_block( std::istream& input, std::string& block )
        {
            for ( size_t i = 0; i < max_block_size; ++i)
            {
                if ( !input )
                {
                    return false;
                }

                block.push_back( input.get() );
                if( block.back() == 0x3 )
                {
                    return true;
                }
            }
            return false;
        }
    }
}

void multicast_communication::tests_::quote_message_tests()
{
    BOOST_CHECK_NO_THROW
    (
        quote_message qm;

        BOOST_CHECK_EQUAL( qm.security_symbol(), "" );
        BOOST_CHECK_EQUAL( qm.bid_price(), 0.0 );
        BOOST_CHECK_EQUAL( qm.bid_volume(), 0.0 );
        BOOST_CHECK_EQUAL( qm.offer_price(), 0.0 );
        BOOST_CHECK_EQUAL( qm.offer_volume(), 0.0 );
        BOOST_CHECK_EQUAL( qm.time(), 0.0 );
    )


    { // test short quote
        std::istringstream input( "EDEO A  003759032T:J_073ADMR  B00004147006 B00004148004 12" );

        quote_message qm;
        BOOST_CHECK_NO_THROW
        (
            input >> qm; 
        )

            BOOST_CHECK_EQUAL( qm.security_symbol(), "ADM" );
            BOOST_CHECK_EQUAL( fabs( qm.bid_price() -  41.47 ) < eps, true );
            BOOST_CHECK_EQUAL( fabs( qm.bid_volume() -  6.0 ) < eps, true );
            BOOST_CHECK_EQUAL( fabs( qm.offer_price() -  41.48 ) < eps, true );
            BOOST_CHECK_EQUAL( fabs( qm.offer_volume() -  4.0 ) < eps, true );
            BOOST_CHECK_EQUAL( qm.time(), 37607u );
    }

    { // test long quote
        std::istringstream input("EBEO A  003759692Y:J_839AVB             0    AAAR BB0000000121290000001B0000000121580000001     A   02");

        quote_message qm;
        BOOST_CHECK_NO_THROW
        (
            input >> qm; 
        )

        BOOST_CHECK_EQUAL( qm.security_symbol(), "AVB" );
        BOOST_CHECK_EQUAL( fabs( qm.bid_price() -  121.29 ) < eps, true );
        BOOST_CHECK_EQUAL( fabs( qm.bid_volume() -  1.0 ) < eps, true );
        BOOST_CHECK_EQUAL( fabs( qm.offer_price() -  121.58 ) < eps, true );
        BOOST_CHECK_EQUAL( fabs( qm.offer_volume() -  1.0 ) < eps, true );
        BOOST_CHECK_EQUAL( qm.time(), 37607u );
    }

    { // test write message
        std::istringstream input( "EDEO A  003759032T:J_073ADMR  B00004147006 B00004148004 12" );

        quote_message qm;
        BOOST_CHECK_NO_THROW( input >> qm; )

        std::ostringstream output;
        BOOST_CHECK_NO_THROW( output << qm ; )
        BOOST_CHECK_EQUAL( output.str(), "Q ADM 41.47 6.0 41.48 4.0\n" );
    }

    {
        std::ifstream input( TEST_DATA_DIR "/233.200.79.0.udp" );
        std::string block;
        quote_message_ptr_list msgs;
        while ( quote_tests_::get_block( input, block ) )
        {
            BOOST_CHECK_NO_THROW( parse_block( block, msgs ); );
        }
        BOOST_CHECK_EQUAL( input.eof(), true );

    }

    {
        std::istringstream input("\x1" "EDEO A  003759032T:J_073ADMR  Z00004147006 B00004148004 12" "\x3");
        std::string block;
        quote_message_ptr_list msgs;
        bool res = true;
        BOOST_CHECK_NO_THROW( res = parse_block( block, msgs ); );
        BOOST_CHECK_EQUAL( res, false );    
    }

    {
        std::istringstream input("\x1" "EDEO A  003759032T:J_073ADMR  B0000sdf7006 B00004148004 12" "\x3");
        std::string block;
        quote_message_ptr_list msgs;
        bool res = true;
        BOOST_CHECK_NO_THROW( res = parse_block( block, msgs ); );
        BOOST_CHECK_EQUAL( res, false );    
    }
    {
        std::istringstream input("\x1""\x3");
        std::string block;
        quote_message_ptr_list msgs;
        bool res = true;
        BOOST_CHECK_NO_THROW( res = parse_block( block, msgs ); );
        BOOST_CHECK_EQUAL( res, false );    
    }

}
