#include "market_data_receiver.h"
#include "async_writer.h"
#include <fstream>
#include <signal.h>

using namespace multicast_communication;

int main()
{
    std::ofstream output( BINARY_DIR "/market.dat" );
    async_writer writer( output );

    market_data_receiver::ports quote_ports;
    quote_ports.push_back( std::make_pair( "233.200.79.0", 61000 ) );
    quote_ports.push_back( std::make_pair( "233.200.79.1", 61001 ) );
    market_data_receiver::ports trade_ports;
    trade_ports.push_back( std::make_pair( "233.200.79.128", 62128 ) );
    trade_ports.push_back( std::make_pair( "233.200.79.129", 62129 ) );
    trade_ports.push_back( std::make_pair( "233.200.79.130", 62130 ) );
    trade_ports.push_back( std::make_pair( "233.200.79.131", 62131 ) );
    market_data_receiver receiver( 4, 2, trade_ports, quote_ports, writer );

    

}
