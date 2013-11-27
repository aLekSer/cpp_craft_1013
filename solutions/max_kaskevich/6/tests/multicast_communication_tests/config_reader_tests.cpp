#include "test_registrator.h"
#include "config_reader.h"
#include <fstream>

void multicast_communication::tests_::config_reader_tests()
{
    BOOST_CHECK_NO_THROW( config_reader config; )
    config_reader config;
    config.read( std::ifstream( TEST_DATA_DIR "/config.ini" ) );
    BOOST_CHECK_EQUAL( config.trade_thread_size, 4 );
    BOOST_CHECK_EQUAL( config.quote_thread_size, 4 );

    BOOST_CHECK_NO_THROW(
        BOOST_CHECK_EQUAL( config.trade_ports[0].first, "233.200.79.0" );
        BOOST_CHECK_EQUAL( config.trade_ports[0].second, 61000 );
        BOOST_CHECK_EQUAL( config.trade_ports[1].first, "233.200.79.1" );
        BOOST_CHECK_EQUAL( config.trade_ports[1].second, 61001 );

        BOOST_CHECK_EQUAL( config.quote_ports[0].first, "233.200.79.128" );
        BOOST_CHECK_EQUAL( config.quote_ports[0].second, 62128 );
        BOOST_CHECK_EQUAL( config.quote_ports[1].first, "233.200.79.129" );
        BOOST_CHECK_EQUAL( config.quote_ports[1].second, 62129 );
        BOOST_CHECK_EQUAL( config.quote_ports[2].first, "233.200.79.130" );
        BOOST_CHECK_EQUAL( config.quote_ports[2].second, 62130 );
        BOOST_CHECK_EQUAL( config.quote_ports[3].first, "233.200.79.131" );
        BOOST_CHECK_EQUAL( config.quote_ports[3].second, 62131 );
        )

    class empty_market_processor : public market_data_processor
    {
    private:
        void new_trade( const trade_message_ptr& ) {}
        void new_quote( const quote_message_ptr& ) {}
    };
    empty_market_processor proc;
    config.processor = &proc;
    receiver_ptr receiver;
    BOOST_CHECK_NO_THROW( receiver = config.generate_receiver(); )
    
    BOOST_CHECK_EQUAL( ( receiver != NULL ) , true );
}