#ifndef _ASYNC_UDP_UDP_LISTENER_H_
#define _ASYNC_UDP_UDP_LISTENER_H_

#include <string>
#include <vector>
#include <memory>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <functional>

namespace multicast_communication
{
    class udp_listener : virtual private boost::noncopyable
    {
        size_t buffer_size_;

        boost::asio::io_service& io_service_;

        boost::asio::ip::udp::endpoint listen_endpoint_;
        boost::asio::ip::udp::socket socket_;

        std::string multicast_address_;

        typedef std::unique_ptr< char[]> buffer_type;
        buffer_type buffer_;
        typedef std::function< void ( const std::string& ) > callback_type;
        callback_type callback_;

        mutable boost::mutex protect_messages_;
        std::vector< std::string > messages_;

    public:
        explicit udp_listener( boost::asio::io_service& io_service, const std::string& multicast_address,
            unsigned short port, callback_type callback, size_t buffer_size = 1000u );
        ~udp_listener();
    private:
        void socket_reload_();
        void register_listen_();
        void listen_handler_( const boost::system::error_code& error, const size_t bytes_received );
    };

    typedef std::shared_ptr< udp_listener > udp_listener_ptr;
}

#endif // _ASYNC_UDP_UDP_LISTENER_H_
