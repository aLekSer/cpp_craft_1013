#include "test_registrator.h"

#include <quote_message.h>
#include <fstream>

static const double eps = 0.0000001;


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


    {
        std::istringstream input("EDEO A  003759032T:J_073ADMR  B00004147006 B00004148004 12");

        quote_message_ptr qm;
        BOOST_CHECK_NO_THROW
        (
            input >> qm; 
        )

            BOOST_CHECK_EQUAL( qm->security_symbol(), std::string("ADM") );
            BOOST_CHECK_EQUAL( fabs( qm->bid_price() -  41.47) < eps, true);
            BOOST_CHECK_EQUAL( fabs( qm->bid_volume() -  6.0) < eps, true);
            BOOST_CHECK_EQUAL( fabs( qm->offer_price() -  41.48) < eps, true);
            BOOST_CHECK_EQUAL( fabs( qm->offer_volume() -  4.0) < eps, true);
    }
}
