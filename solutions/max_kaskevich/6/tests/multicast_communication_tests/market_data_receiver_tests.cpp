#include "test_registrator.h"

#include "market_data_receiver.h"
#include "async_writer.h"
#include <fstream>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <clocale>

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

        void get_quote_msgs_ouput( const std::string& block, std::list< std::string >& msgs_ouput );
        void get_trade_msgs_ouput( const std::string& block, std::list< std::string >& msgs_ouput );

        void test_sender( const std::string address, unsigned short port, std::list< std::string >& msgs_ouput,
            std::function< void ( const std::string&, std::list< std::string >& ) > block_handler);

        class market_data_processor_test_helper : public market_data_processor
        {
        public:
            explicit market_data_processor_test_helper(){}
            virtual ~market_data_processor_test_helper(){}
        private:
            virtual void new_trade( const trade_message_ptr& );
            virtual void new_quote( const quote_message_ptr& );
        };

        boost::mutex mtx;
    }
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

void multicast_communication::tests_::get_quote_msgs_ouput( const std::string& block, std::list< std::string >& msgs_ouput )
{
    quote_message_ptr_list msgs;
    quote_message::parse_block( block, msgs );
    for( auto msg: msgs )
    {
        std::ostringstream output;
        output << *( msg );
        boost::mutex::scoped_lock lock(mtx);
        msgs_ouput.push_back( output.str() );
    }
}
void multicast_communication::tests_::get_trade_msgs_ouput( const std::string& block, std::list< std::string >& msgs_ouput )
{
    trade_message_ptr_list msgs;
    trade_message::parse_block( block, msgs );
    for( auto msg: msgs )
    {
        std::ostringstream output;
        output << *( msg );
        boost::mutex::scoped_lock lock(mtx);
        msgs_ouput.push_back( output.str() );
    }
}


void multicast_communication::tests_::test_sender( const std::string address, unsigned short port,
                                                  std::list< std::string >& msgs_ouput,
                                                  std::function< void ( const std::string&, std::list< std::string >& ) > block_handler)
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
    boost::thread receive_messages( boost::bind( detail::service_thread, boost::ref( service ) ) );

    std::string block;
    while( get_block( input, block ) )
    {
        socket.send_to( boost::asio::buffer( block ), endpoint );
        block_handler( block, msgs_ouput );
    }
    
    BOOST_CHECK_EQUAL( input.eof(), true );
    socket.close();
    //boost::this_thread::sleep_for( boost::chrono::milliseconds( 2000 ) );
    service.stop();
    receive_messages.join();
}

void multicast_communication::tests_::market_data_receiver_tests()
{
     std::setlocale( LC_ALL, "" );
	//BOOST_CHECK_NO_THROW
	//( 
        std::ostringstream output;
		async_writer writer( output );

        market_data_receiver::ports quote_ports;
        quote_ports.push_back( std::make_pair( "233.200.79.0", 61000 ) );
        quote_ports.push_back( std::make_pair( "233.200.79.1", 61001 ) );
        market_data_receiver::ports trade_ports;
        trade_ports.push_back( std::make_pair( "233.200.79.128", 62128 ) );
        trade_ports.push_back( std::make_pair( "233.200.79.129", 62129 ) );
        trade_ports.push_back( std::make_pair( "233.200.79.130", 62130 ) );
        trade_ports.push_back( std::make_pair( "233.200.79.131", 62131 ) );
        market_data_receiver receiver( 4, 4, trade_ports, quote_ports, writer );

        boost::thread_group test_threads;

        std::list< std::string > msgs_ouput;

        for ( auto point: quote_ports )
        {
            test_threads.create_thread(
                bind( test_sender, point.first, point.second, ref( msgs_ouput ), get_quote_msgs_ouput ) );
        }

        for ( auto point: trade_ports )
        {
            test_threads.create_thread(
                bind( test_sender, point.first, point.second, ref( msgs_ouput ), get_trade_msgs_ouput ) );
        }

        test_threads.join_all();
        receiver.stop();
        writer.stop();

        std::string raw_output = output.str();
        for(auto msg_output: msgs_ouput)
        {
            boost::replace_first( raw_output, msg_output, "" );
        }

        BOOST_CHECK_EQUAL( raw_output.empty(), true );

	//)
}
