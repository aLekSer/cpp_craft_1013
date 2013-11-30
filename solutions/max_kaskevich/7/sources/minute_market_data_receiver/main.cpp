#include "market_data_receiver.h"
#include "async_writer.h"
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

using namespace multicast_communication;

int main()
{

    std::signal( SIGINT, signal_handler );
    boost::mutex::scoped_lock lock( mtx );
    cond.wait( lock );
}
