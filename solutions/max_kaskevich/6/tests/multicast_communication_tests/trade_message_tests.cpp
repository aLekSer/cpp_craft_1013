#include "test_registrator.h"

#include <trade_message.h>

static const double eps = 0.0000001;

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
}
