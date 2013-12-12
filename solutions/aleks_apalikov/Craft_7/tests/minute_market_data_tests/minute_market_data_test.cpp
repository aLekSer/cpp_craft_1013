#include "test_registrator.h"
#include "minute_market_data.h"

void market_data::data_test()
{
	{
		boost::asio::io_service service;
		boost::asio::ip::udp::endpoint endp( boost::asio::ip::address::from_string( "233.200.79.0" ), 61000 ); 
		boost::asio::ip::udp::socket socket( service, endp.protocol() );

		stringstream ss;
		ss << "\x01" << "EBEO A  003759557N:J_735AVB             0    AAAR B30000012127000000001D0000012137000000001     A   62TB00012130001 BB00012137001"
			<<"\x1f" << "EDEO A  003759121P:J_428AINR  D00352000001 F00354300001 02"
			<<"\x1f" << "LDEO A  003759122N:K_432ALJR  F00124900003 D00125100001 02" << "\x03";
		string str = ss.str();
		socket.send_to( boost::asio::buffer( str.c_str(), str.size() ), endp );
		minute_market_data md;
		md.run();
		int i = 0;
		while( i < 100 )
		{
			socket.send_to( boost::asio::buffer( str.c_str(), str.size() ), endp );
			boost::this_thread::sleep_for( boost::chrono::nanoseconds( 10000 ) );
			i++;
//			md.process_one();
		}
		boost::this_thread::sleep_for( boost::chrono::nanoseconds( 1000000000 ) );
		md.stop();
	
	
	}
}
