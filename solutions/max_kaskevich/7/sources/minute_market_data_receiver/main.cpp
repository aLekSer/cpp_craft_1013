#include "market_data_receiver.h"
#include "minute_market_data.h"
#include "config_reader.h"
#include <fstream>
#include <csignal>
#include <boost/thread.hpp>

boost::mutex mtx;
boost::condition_variable cond;

void signal_handler( int )
{
    cond.notify_all();
}

using namespace minute_market;
using namespace multicast_communication;

int main()
{

    minute_market_data processor( BINARY_DIR );
    config_reader config;
    if ( !config.read( std::ifstream( BINARY_DIR "/config.ini" ) ) )
    {
        std::cout << "Can not find config.ini" << std::endl;
        return -1;
    }
    config.processor = &processor;
    receiver_ptr receiver = config.generate_receiver();  
    std::signal( SIGINT, signal_handler );
    boost::mutex::scoped_lock lock( mtx );
    cond.wait( lock );
}
