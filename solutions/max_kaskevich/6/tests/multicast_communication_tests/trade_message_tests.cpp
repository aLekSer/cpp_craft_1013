#include "test_registrator.h"

#include <trade_message.h>
#include <fstream>

static const double eps = 0.0000001;

namespace multicast_communication
{
    static size_t max_block_size = 1000u;
    namespace trade_tests_
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

void multicast_communication::tests_::trade_message_tests()
{
	BOOST_CHECK_NO_THROW
	(
		trade_message tm;

		BOOST_CHECK_EQUAL( tm.security_symbol(), "" );
		BOOST_CHECK_EQUAL( tm.price(), 0.0 );
		BOOST_CHECK_EQUAL( tm.volume(), 0.0 );
	)


    { // test short trade
        std::istringstream input( "EIAO A  000146235T:3]008ACN@0100B00007790DD " );

        trade_message tm;
        BOOST_CHECK_NO_THROW( input >> tm; )

        BOOST_CHECK_EQUAL( tm.security_symbol(), "ACN" );
        BOOST_CHECK_EQUAL( fabs( tm.price() -  77.90 ) < eps, true );
        BOOST_CHECK_EQUAL( fabs( tm.volume() -  100.0 ) < eps, true );
    }

    { // test long trade
        std::istringstream input( "EBAO A  000146234N:3]004ACN             0     000 F  1  D000000779000000000100DD 0" );

        trade_message tm;
        BOOST_CHECK_NO_THROW
            (
            input >> tm; 
        )

        BOOST_CHECK_EQUAL( tm.security_symbol(), "ACN" );
        BOOST_CHECK_EQUAL( fabs( tm.price() -  77.90 ) < eps, true );
        BOOST_CHECK_EQUAL( fabs( tm.volume() -  100.0 ) < eps, true );
    }

    { // test write trade
        std::istringstream input( "EIAO A  000146235T:3]008ACN@0100B00007790DD " );

        trade_message tm;
        BOOST_CHECK_NO_THROW( input >> tm; )

        std::ostringstream output;
        BOOST_CHECK_NO_THROW( output << tm ; )
        BOOST_CHECK_EQUAL( output.str(), "T ACN 77.90 100.0\n" );
    }

    {
        std::ifstream input( TEST_DATA_DIR "/233.200.79.128.udp" );
        std::string block;
        trade_message_ptr_list msgs;
        while ( trade_tests_::get_block( input, block ) )
        {
            BOOST_CHECK_NO_THROW( trade_message::parse_block(block, msgs ); );
        }
        BOOST_CHECK_EQUAL( input.eof(), true );

    }
}
