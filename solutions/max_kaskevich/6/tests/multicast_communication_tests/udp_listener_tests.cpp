#include "test_registrator.h"

#include <udp_listener.h>

#include <cstring>

namespace multicast_communication
{
    namespace tests_
    {
        namespace detail
        {
            void service_thread( boost::asio::io_service& service );
        }
    }
}

void multicast_communication::tests_::detail::service_thread( boost::asio::io_service& service )
{
    service.run();
}

void multicast_communication::tests_::udp_listener_tests()
{
    BOOST_CHECK_NO_THROW
    ( 
        boost::asio::io_service service;
        udp_listener uw( service, "224.0.0.0", 50000, []( const std::string& str) -> void {} );
    );

    {
        boost::mutex mtx;
        boost::condition_variable cond;
        bool callback_checked = false;
        boost::asio::io_service service;
        udp_listener uw( service, "224.0.0.0", 50000, [&]( const std::string& str) -> void {
            BOOST_CHECK_EQUAL( strcmp( str.c_str(), "hello world" ), 0 );
            callback_checked = true;
            cond.notify_one();
        } );

        const std::string buffer( "hello world" );
        boost::asio::ip::udp::endpoint endpoint( boost::asio::ip::address::from_string( "224.0.0.0" ), 50000 ); 
        boost::asio::ip::udp::socket socket( service, endpoint.protocol() );
        boost::thread receive_messages( boost::bind( detail::service_thread, boost::ref( service ) ) );
        socket.send_to( boost::asio::buffer( buffer ), endpoint );
        boost::unique_lock< boost::mutex > lock( mtx );

        auto timeout = boost::chrono::milliseconds( 2000 );
        while ( !callback_checked && cond.wait_for( lock, timeout ) == boost::cv_status::no_timeout )
        {}

        service.stop();
        receive_messages.join();
        BOOST_CHECK_EQUAL( callback_checked, true );
    }
}
