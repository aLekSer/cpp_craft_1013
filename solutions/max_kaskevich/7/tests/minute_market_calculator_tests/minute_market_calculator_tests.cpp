#include "test_registrator.h"

#include <boost/test/unit_test.hpp>
#include "minute_market_calculator.h"

using namespace multicast_communication;

 void minute_market::tests_::minute_market_calculator_tests()
 {
     {
         BOOST_CHECK_NO_THROW( minute_market_calculator calc([]( const minute_datafeed& mdf ){}); )
     }

     {
        bool callback_checked = false;
        minute_market_calculator calc( [&]( const minute_datafeed& mdf )
        {
            BOOST_CHECK_EQUAL( mdf.close_prise, 88.9 );
            BOOST_CHECK_EQUAL( mdf.open_prise, 77.9 );
            BOOST_CHECK_EQUAL( mdf.high_prise, 88.9 );
            BOOST_CHECK_EQUAL( mdf.low_price, 77.9 );
            BOOST_CHECK_EQUAL( std::string( mdf.stock_name ), std::string( "ACN" ) );
            BOOST_CHECK_EQUAL( mdf.minute, 36225u );
            BOOST_CHECK_EQUAL( mdf.bid, 12 );
            BOOST_CHECK_EQUAL( mdf.ask, 8.0 );
            std::ostringstream output;
            BOOST_CHECK_NO_THROW( output << mdf; )
            BOOST_CHECK_EQUAL( output.str().length(), 76 );
            callback_checked = true;
        });

        trade_message_ptr tm1( new trade_message() );
        {
            std::istringstream input( "EIAO A  000146235T:3]008ACN@0100B00007790DD " );
            input >> *tm1;
        }
        trade_message_ptr tm2( new trade_message() );
        {
            std::istringstream input( "EIAO A  000146235T:3b008ACN@0100B00008890DD " );
            input >> *tm2;
        }
        trade_message_ptr tm3( new trade_message() );
        {
            std::istringstream input( "EIAO A  000146235T:5]008ACN@0100B00008890DD " );
            input >> *tm3;
        }

        quote_message_ptr qm( new quote_message() );
        {
            std::istringstream input( "EDEO A  003759032T:3]073ACNR  B00004147006 B00004148004 12" );
            input >> *qm;
        }
        quote_message_ptr qm2( new quote_message() );
        {
            std::istringstream input( "EDEO A  003759032T:3]073ACNR  B00004147006 B00004148004 12" );
            input >> *qm2;
        }

        calc.new_trade( tm1 );
        calc.new_quote( qm );
        calc.new_quote( qm2 );
        calc.new_trade( tm2 );
        BOOST_CHECK_EQUAL( callback_checked, false );
        calc.new_trade( tm3 );
        BOOST_CHECK_EQUAL( callback_checked, true );

    }
 }
