#include "test_registrator.h"

#include <trade_message.h>

void multicast_communication::tests_::trade_message_tests()
{
	BOOST_CHECK_NO_THROW
	(
		trade_message tm;

		BOOST_CHECK_EQUAL( tm.security_symbol(), "" );
		BOOST_CHECK_EQUAL( tm.price(), 0.0 );
		BOOST_CHECK_EQUAL( tm.volume(), 0.0 );
	)

	BOOST_CHECK_NO_THROW
	(
		trade_message tm("ss", 84.4, 78.6);

		BOOST_CHECK_EQUAL( tm.security_symbol(), "ss" );
		BOOST_CHECK_EQUAL( tm.price(), 84.4 );
		BOOST_CHECK_EQUAL( tm.volume(), 78.6 );
	)
}
