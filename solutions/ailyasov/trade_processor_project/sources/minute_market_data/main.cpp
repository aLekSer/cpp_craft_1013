#include <vector>
#include <utility>
#include <fstream>
#include <iostream>

#include <signal.h>

#include <boost/thread.hpp>

#include "market_data_receiver.h"
#include "market_data_processor.h"
#include "minute_calculator.h"

#include "property.h"

using namespace multicast_communication;

static boost::condition_variable cond_var; 

static void sighandler(int)
{
    cond_var.notify_one();
}

template <typename T> static void save(std::ofstream& out, T const & v)
{
    out.write(reinterpret_cast<const char*>(&v), sizeof( T ) );
}

int main()
{ 
    using namespace minute_calculator;

    signal(SIGABRT, &sighandler);
    signal(SIGTERM, &sighandler);
    signal(SIGINT, &sighandler); 

    property::reader reader(SOURCE_DIR "/config.ini");
    if(reader.opened())
    { 
        boost::mutex ctrl_c; 

        market_data_processor processor; 
        market_data_receiver receiver(
                  reader.trade_thread_size()
                , reader.quote_thread_size()
                , reader.trades()
                , reader.quotes()
                , processor );

        receiver.start(); 
        calculator calc(processor.trades(), processor.quotes());
        calc.start();

        boost::mutex::scoped_lock lock(ctrl_c);
        cond_var.wait(lock); 
        receiver.stop();
        calc.stop();
        std::map<std::string, std::vector< minute_datafeed_ptr> > data = calc.get_minute_datafeed();
        for(std::map<std::string, std::vector< minute_datafeed_ptr> >::iterator i = data.begin(); i != data.end(); ++i)
        {
            std::ofstream out(BINARY_DIR "/" + i->first + ".dat", std::ios_base::out | std::ios_base::binary);
            std::vector< minute_datafeed_ptr > const& v = i->second;
            for(std::vector< minute_datafeed_ptr >::const_iterator j = v.begin(); j != v.end(); ++j)
            { 
                minute_datafeed_ptr p = *j;
                save(out, p->minute_);
                save(out, p->open_price_);
                save(out, p->high_price_);
                save(out, p->low_price_);
                save(out, p->close_price_);
                save(out, p->volume_);
                save(out, p->bid_);
                save(out, p->ask_);
                std::cout << i->first << " " << *p << std::endl;
            }
        }
    } else 
    {
        std::cerr << "Unable to open config file at " SOURCE_DIR " /config.ini" << std::endl;
    }
}
