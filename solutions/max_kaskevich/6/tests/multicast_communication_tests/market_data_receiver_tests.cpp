#include "test_registrator.h"

#include "market_data_receiver.h"
#include "async_writer.h"
#include <fstream>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <clocale>
#include <boost//filesystem.hpp>

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

        void write_quotes( const std::string& block, std::ofstream& test_output );
        void write_trades( const std::string& block, std::ofstream& test_output );

        void test_sender( const std::string& address, unsigned short port, std::ofstream& test_output,
            std::function< void ( const std::string&, std::ofstream& ) > block_handler);

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

bool multicast_communication::tests_::get_block( std::istream& input, std::string& block )
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

void multicast_communication::tests_::write_quotes( const std::string& block, std::ofstream& test_output )
{
    boost::mutex::scoped_lock lock( test_output_mtx );
    quote_message_ptr_list msgs;
    BOOST_CHECK_NO_THROW ( parse_block( block, msgs ); )
    for( auto msg: msgs )
    {
        BOOST_CHECK_NO_THROW( test_output << *( msg ); )
    }
}
void multicast_communication::tests_::write_trades( const std::string& block, std::ofstream& test_output )
{
    boost::mutex::scoped_lock lock( test_output_mtx );
    trade_message_ptr_list msgs;
    BOOST_CHECK_NO_THROW (  parse_block( block, msgs ); )
    for( auto msg: msgs )
    {
        BOOST_CHECK_NO_THROW( test_output << *( msg ); )
    }
}


void multicast_communication::tests_::test_sender( const std::string& address, unsigned short port,
    std::ofstream& test_output, std::function< void ( const std::string&, std::ofstream& ) > block_handler )
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
        //boost::this_thread::sleep_for( boost::chrono::milliseconds( 20 ) );
        block_handler( block, test_output );
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

        const std::string test_ouput_path = BINARY_DIR "/test.output";
        const std::string async_writer_path = BINARY_DIR "/async_writer.output";
        std::ofstream test_output( test_ouput_path );
        std::ofstream output( async_writer_path );
        async_writer writer( output );

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
        market_data_receiver receiver( 16, 16, trade_ports, quote_ports, writer );

        boost::thread_group test_threads;

        for ( auto point: quote_ports )
        {
            test_threads.create_thread(
                boost::bind( test_sender, point.first, point.second, boost::ref( test_output ), write_quotes ) );
        }

        for ( auto point: trade_ports )
        {
            test_threads.create_thread(
                boost::bind( test_sender, point.first, point.second, boost::ref( test_output ), write_trades ) );
        }

        test_threads.join_all();

        //boost::this_thread::sleep_for( boost::chrono::milliseconds( 2000 ) );
        BOOST_CHECK_NO_THROW( receiver.stop(); )
        BOOST_CHECK_NO_THROW( writer.stop(); )

        test_output.close();
        output.close();
        using boost::filesystem::file_size;
        using boost::filesystem::path;
        BOOST_CHECK_EQUAL( file_size( test_ouput_path ), file_size( async_writer_path ) );
        //boost::filesystem::remove( path( test_ouput_path ) );
        //boost::filesystem::remove( path( async_writer_path ) );
    //)
}
