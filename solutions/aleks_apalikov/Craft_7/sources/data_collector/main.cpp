#include "message_parser.h"
#include <iostream>
#include <string>
#include <csignal>
#include "Stock_receiver.h"

#include "main.h" // for tests only

using namespace std;
bool stop = false;
void int_handler(int)
{
	stop = true;
}

int main ( int argc, char* argv[] )
{
	signal(SIGINT, int_handler);
#ifdef _data_collector_test_
	boost::thread send(&start_sending);
#endif

	minute_market_data md;
	cout << "Data_collector is running" << endl;
	while (!stop)
	{
		boost::this_thread::sleep_for( boost::chrono::milliseconds( 50 ) );
	}

	md.stop();
	cout << "Program was interrupted, market data saved in market_data.dat output file, "
		"check stats in sources/data/stats dir "<< endl;


	return 0;
}
