#include "test_registrator.h"

#include <quote_message.h>

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
	)

	BOOST_CHECK_NO_THROW
	(
		quote_message qm("ss", 45.6, 35.7, 65.3, 95.3);

		BOOST_CHECK_EQUAL( qm.security_symbol(), "ss" );
		BOOST_CHECK_EQUAL( qm.bid_price(), 45.6 );
		BOOST_CHECK_EQUAL( qm.bid_volume(), 35.7 );
		BOOST_CHECK_EQUAL( qm.offer_price(), 65.3 );
		BOOST_CHECK_EQUAL( qm.offer_volume(), 95.3 );
	)
}
