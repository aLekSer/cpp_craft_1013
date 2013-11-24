#include "test_registrator.h"

#include "market_data_receiver.h"
#include "async_writer.h"
#include <fstream>
#include <boost/format.hpp>

namespace multicast_communication
{
    static size_t max_block_size = 1000u;
    namespace tests_
    {
        namespace detail
        {
            void service_thread( boost::asio::io_service& service );
        }

        bool get_block( std::istream& input, std::string& block );

        void test_sender( const std::string address, unsigned short port);

    }
}

void multicast_communication::tests_::detail::service_thread( boost::asio::io_service& service )
{
    service.run();
}

bool multicast_communication::tests_::get_block( std::istream& input, std::string& block )
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

void multicast_communication::tests_::test_sender( const std::string address, unsigned short port)
{
    std::ifstream input ( ( boost::format( TEST_DATA_DIR "/%1%.udp" ) % address ).str() );
    BOOST_CHECK_EQUAL( input, true );
    if ( !input )
    {
        return;
    }

    boost::asio::io_service service;

    boost::asio::ip::udp::endpoint endpoint( boost::asio::ip::address::from_string( address ), port );
    boost::asio::ip::udp::socket socket( service, endpoint.protocol() );
    boost::thread receive_messages( boost::bind( detail::service_thread, boost::ref( service ) ) );

    std::string block;
    std::list< std::string > msgs_ouput;
    while( get_block( input, block ) )
    {
        socket.send_to( boost::asio::buffer( block ), endpoint );
        quote_message_ptr_list msgs;
        quote_message::parse_block( block, msgs);
        for( auto msg: msgs)
        {
            std::ostringstream output;
            output << msg;
            msgs_ouput.push_back( output.str() );
        }
    }
    
    BOOST_CHECK_EQUAL( input.eof(), true );
    service.stop();
    receive_messages.join();

    // lock
}

void multicast_communication::tests_::market_data_receiver_tests()
{
	//BOOST_CHECK_NO_THROW
	//( 
        std::ostringstream output;
		async_writer writer( output );

        market_data_receiver::ports quote_ports;
        quote_ports.push_back( std::make_pair( "233.200.79.0", 61000 ) );
        quote_ports.push_back( std::make_pair( "233.200.79.1", 61001 ) );
        market_data_receiver::ports trade_ports;
        quote_ports.push_back( std::make_pair( "233.200.79.128", 62128 ) );
        quote_ports.push_back( std::make_pair( "233.200.79.129", 62129 ) );
        quote_ports.push_back( std::make_pair( "233.200.79.128", 62128 ) );
        quote_ports.push_back( std::make_pair( "233.200.79.129", 62129 ) );
        market_data_receiver receiver( 4, 4, quote_ports, trade_ports, writer );


	//)
}
