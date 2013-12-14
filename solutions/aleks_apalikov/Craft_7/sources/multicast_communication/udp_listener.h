#ifndef _ASYNC_UDP_UDP_LISTENER_H_
#define _ASYNC_UDP_UDP_LISTENER_H_

#include <string>
#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "Stock_receiver.h"

class stock_receiver;
class callable_obj
{
	stock_receiver* sr;
	bool quotes;
public:
	callable_obj(stock_receiver* st_rec = NULL, bool quotes = true);
	void operator() (boost::shared_ptr<string> buf);
};

	class udp_listener : virtual private boost::noncopyable
	{
		static const size_t default_buffer_size;
		callable_obj* callback;

		boost::asio::io_service& io_service_;

		boost::asio::ip::udp::endpoint listen_endpoint_;
		boost::asio::ip::udp::socket socket_;

		std::string multicast_address_;

		typedef boost::shared_ptr< std::string > buffer_type;

		mutable boost::mutex protect_messages_;
		std::vector< std::string > messages_;

	public:
		explicit udp_listener( boost::asio::io_service& io_service, const std::string& multicast_address, unsigned short port, callable_obj* co = NULL);
		~udp_listener();
		const std::vector< std::string > messages() const;
		boost::shared_ptr<std::string> messages_pop()
		{
			boost::shared_ptr<std::string> sp (new std::string );
			sp.reset(new std::string(messages_.back()));
			messages_.pop_back();
			return sp;
		}

		boost::mutex & protect_messages()
		{
			return protect_messages_;
		}
	private:
		void socket_reload_();
		void register_listen_( buffer_type pre_buffer = buffer_type(), const size_t previous_size = 0 );
		void listen_handler_( buffer_type bt, const boost::system::error_code& error, const size_t bytes_received );
		static void enlarge_buffer_( buffer_type& bt );
	};


#endif // _ASYNC_UDP_UDP_LISTENER_H_
