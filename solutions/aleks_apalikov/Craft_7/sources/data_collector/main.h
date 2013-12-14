#ifndef _data_collector_test_
#define _data_collector_test_
#include "Stock_receiver.h"
#include "config.h"
#include <sstream>
#include "market_data_processor.h"
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include "config.h"
#include <fstream>

typedef vector<ifstream*> vec_ifstr;
typedef vector<ifstream*>::iterator f_iter;
typedef boost::asio::ip::udp::endpoint endpoint;
typedef vector<endpoint>::iterator e_iter;
void start_sending()
{
	boost::asio::io_service service;
	boost::asio::ip::udp::endpoint endp( boost::asio::ip::address::from_string( "233.200.79.128" ), 62128 ); 
	boost::asio::ip::udp::socket socket( service, endp.protocol() );
	config c(data_path + string("config.ini"));
	typedef addresses::const_iterator iter;
	using namespace boost::asio::ip;

	vector<endpoint> trades;
	vec_ifstr trade_files;
	for (iter i = c.get_trades().begin(); i != c.get_trades().end(); ++i )
	{
		trades.push_back( endpoint(address::from_string( i->first ), i->second ));
		trade_files.push_back(new ifstream (string(data_path + i->first + ".udp").c_str()) );
	}
	vector<endpoint> quotes;
	vec_ifstr quote_files;
	for (iter q = c.get_quotes().begin(); q != c.get_quotes().end(); ++q )
	{
		quotes.push_back( endpoint(address::from_string( q->first ), q->second ));
		quote_files.push_back(new ifstream ( string(data_path + q->first + ".udp").c_str()) );
	}

	while (!all_empty(quote_files, trade_files))
	{
		f_iter tf = trade_files.begin();
		for (e_iter t = trades.begin(); t != trades.end(); t++, tf++ )
		{
			stringstream ss;
			if((*tf)->peek() != EOF)
			{
				message::read_block(ss, **tf);
				socket.send_to( boost::asio::buffer( ss.str().c_str(), ss.str().size() ), *t );
				ss.str("");
			}				
		}
		f_iter qf = quote_files.begin();
		for (e_iter q = quotes.begin(); q != quotes.end(); q++, qf++ )
		{
			stringstream ss;
			if((*qf)->peek() != EOF)
			{		
				message::read_block(ss, **qf);
				socket.send_to( boost::asio::buffer( ss.str().c_str(), ss.str().size() ), *q );
				ss.str("");
			}	
		}
		boost::this_thread::sleep_for( boost::chrono::nanoseconds( 100 ) );
	}

	clear_files(trade_files, quote_files);
	cout << "Total messages was parsed and passed: " << message::count << endl; 

}
#endif