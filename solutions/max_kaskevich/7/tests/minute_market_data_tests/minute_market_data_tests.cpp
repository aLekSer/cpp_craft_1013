#include "test_registrator.h"

#include <boost/test/unit_test.hpp>
#include "minute_market_data.h"
#include "market_data_receiver.h"
#include <clocale>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>


using namespace multicast_communication;

namespace minute_market
{
    static size_t max_block_size = 1000u;
    namespace tests_
    {

        bool get_block( std::istream& input, std::string& block );

        void test_sender( const std::string& address, unsigned short port );

        class market_data_processor_test_helper : public market_data_processor
        {
        public:
            explicit market_data_processor_test_helper(){}
            virtual ~market_data_processor_test_helper(){}
        private:
            virtual void new_trade( const trade_message_ptr& );
            virtual void new_quote( const quote_message_ptr& );
        };

        boost::mutex test_output_mtx;
    }
}

bool minute_market::tests_::get_block( std::istream& input, std::string& block )
{
    block.clear();
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

void minute_market::tests_::test_sender( const std::string& address, unsigned short port )
{
    std::ifstream input ( ( boost::format( TEST_DATA_DIR "/%1%.udp" ) % address ).str() );
    BOOST_CHECK_EQUAL( input.is_open(), true );
    if ( !input )
    {
        return;
    }

    boost::asio::io_service service;

    boost::asio::ip::udp::endpoint endpoint( boost::asio::ip::address::from_string( address ), port );
    boost::asio::ip::udp::socket socket( service, endpoint.protocol() );
    boost::thread receive_messages( boost::bind( &boost::asio::io_service::run, &service ) );

    std::string block;
    while( get_block( input, block ) )
    {
        socket.send_to( boost::asio::buffer( block ), endpoint );
        boost::this_thread::sleep_for( boost::chrono::milliseconds( 10 ) );
    }
    BOOST_CHECK_EQUAL( input.eof(), true );
    socket.close();
    service.stop();
    receive_messages.join();
}

 void minute_market::tests_::minute_market_data_tests()
 {
     std::setlocale( LC_ALL, "" );
     //BOOST_CHECK_NO_THROW
     //( 
     minute_market_data processor( BINARY_DIR );

     ports quote_ports;
     quote_ports.push_back( std::make_pair( "233.200.79.0", 61000 ) );
     quote_ports.push_back( std::make_pair( "233.200.79.1", 61001 ) );
     quote_ports.push_back( std::make_pair( "233.200.79.2", 61002 ) );
     quote_ports.push_back( std::make_pair( "233.200.79.3", 61003 ) );
     quote_ports.push_back( std::make_pair( "233.200.79.4", 61004 ) );
     quote_ports.push_back( std::make_pair( "233.200.79.5", 61005 ) );
     quote_ports.push_back( std::make_pair( "233.200.79.6", 61006 ) );
     quote_ports.push_back( std::make_pair( "233.200.79.7", 61007 ) );
     ports trade_ports;
     trade_ports.push_back( std::make_pair( "233.200.79.128", 64128 ) );
     trade_ports.push_back( std::make_pair( "233.200.79.129", 64129 ) );
     trade_ports.push_back( std::make_pair( "233.200.79.130", 64130 ) );
     trade_ports.push_back( std::make_pair( "233.200.79.131", 64131 ) );
     trade_ports.push_back( std::make_pair( "233.200.79.132", 64132 ) );
     trade_ports.push_back( std::make_pair( "233.200.79.133", 64133 ) );
     trade_ports.push_back( std::make_pair( "233.200.79.134", 64134 ) );
     trade_ports.push_back( std::make_pair( "233.200.79.135", 64135 ) );
     market_data_receiver receiver( 16, 16, trade_ports, quote_ports, processor );

     boost::thread_group test_threads;

     for ( auto point: quote_ports )
     {
         test_threads.create_thread(
             boost::bind( test_sender, point.first, point.second ) );
     }

     for ( auto point: trade_ports )
     {
         test_threads.create_thread(
             boost::bind( test_sender, point.first, point.second ) );
     }

     test_threads.join_all();

     //boost::this_thread::sleep_for( boost::chrono::milliseconds( 2000 ) );
     BOOST_CHECK_NO_THROW( receiver.stop(); )
     //)
 }
