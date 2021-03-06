#include "udp_listener.h"

#include <boost/bind.hpp>

const size_t udp_listener::default_buffer_size = 1000ul;

udp_listener::udp_listener( boost::asio::io_service& io_service, const std::string& multicast_address, unsigned short port, callable_obj* co )
	: io_service_( io_service )
	, listen_endpoint_( boost::asio::ip::address::from_string( "0.0.0.0" ), port )
	, socket_( io_service_ )
	, multicast_address_( multicast_address )
	, callback(co)
{
	socket_reload_();
	register_listen_();
}
udp_listener::~udp_listener()
{
}
const std::vector< std::string > udp_listener::messages() const
{
	boost::mutex::scoped_lock lock( protect_messages_ );
	return messages_;
}

void udp_listener::socket_reload_()
{
	using boost::asio::ip::udp;
	using boost::asio::ip::address;
	using boost::asio::ip::multicast::join_group;

	socket_.open( listen_endpoint_.protocol() );
	socket_.set_option( udp::socket::reuse_address( true ) );
	socket_.bind( listen_endpoint_ );
	socket_.set_option( join_group( address::from_string( multicast_address_ ) ) );
}
void udp_listener::register_listen_( buffer_type pre_buffer, const size_t previous_size )
{
	buffer_type buffer;
	
	if( pre_buffer )
		buffer = pre_buffer;
	else
		buffer.reset( new std::string( default_buffer_size, '\0' ) );

	char* const buffer_start = &(*buffer->begin());

	using namespace boost::asio::placeholders;
	socket_.async_receive( boost::asio::buffer( buffer_start, previous_size + default_buffer_size ), 
		boost::bind( &udp_listener::listen_handler_, this, buffer, error, bytes_transferred ) );
}

void udp_listener::listen_handler_( buffer_type bt, const boost::system::error_code& error, const size_t bytes_received )
{
	if ( error )
	{
		static const int NO_ENOUGHT_BUFFER = 234;
		if ( error.value() == NO_ENOUGHT_BUFFER )
		{
			enlarge_buffer_( bt );
			register_listen_( bt, bytes_received );
		}
		return;
	}
	if ( bytes_received == bt->size() && (*bt)[ bytes_received - 1 ] != '\0' )
	{
		enlarge_buffer_( bt );
		register_listen_( bt, bytes_received );
	}
	else
	{
		{
			boost::mutex::scoped_lock lock( protect_messages_ );
			if(callback == NULL)
				messages_.push_back( std::string( bt->c_str(), bytes_received ) );
			else
			{
				boost::shared_ptr<std::string> sp(new std::string( bt->c_str(), bytes_received ));
				(*callback)(sp);
			}
		}
		register_listen_();
	}
}

void udp_listener::enlarge_buffer_( buffer_type& bt )
{
	bt->resize( bt->size() + default_buffer_size );
}

callable_obj::callable_obj( stock_receiver* st_rec , bool is_quot ): quotes (is_quot)
{
	sr = st_rec;
}

void callable_obj::operator()( boost::shared_ptr<string> buf )
{
	if(sr != NULL)
		sr->write_buf(buf, quotes);
}
