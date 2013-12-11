#include <conio.h>

#include <fstream>
#include <iostream>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

#include "market_data_receiver.h"
#include "../multicast_communication/market_messages_pump.h"
#include "../multicast_communication/market_data_processor.h"

namespace multicast_communication
{
  class settings
  {
  public:
    size_t trade_thread_size;
    size_t quote_thread_size;

    std::vector<connection_point> quote_connections;
    std::vector<connection_point> trade_connections;

  public:
    static bool read_connections(std::ifstream& ifs, 
            std::vector<connection_point>& connections);

  public:
    settings() 
    {
      clear();
    }
    void clear()
    {
      trade_thread_size = 0;
      quote_thread_size = 0;
      quote_connections.clear();
      trade_connections.clear();
    }
  };

  void io_thread(boost::asio::io_service& service);

  bool read_settings(settings& s);
  void save_results(market_data_processor_ptr& mdp);
}


int main()
{
  using namespace multicast_communication;

  settings s;
  if (!read_settings(s))
  {
    std::cerr << "multicast_communication::read_settings failed\n";
    return -1;
  }


  boost::asio::io_service trade_io_service;  
  boost::asio::io_service quote_io_service;  
  market_data_processor_ptr market_data_processor(new market_data_processor_impl());
  market_data_receiver_ptr market_data_receiver(new market_data_receiver(market_data_processor));

  market_data_received_delegate_ptr tmessage_received_delegete(new tmessage_received_delegate(market_data_receiver));
  market_data_received_delegate_ptr qmessage_received_delegete(new qmessage_received_delegate(market_data_receiver));

  boost::thread_group io_workers;
  
  std::vector<market_messages_pump_ptr> trade_messages_pumps;
  std::vector<market_messages_pump_ptr> quote_messages_pumps;

  for (size_t i = 0; i < s.trade_connections.size(); ++i)
  {
    const connection_point& trade_cp = s.trade_connections[i];
    market_messages_pump_ptr tmessage_pump(new market_messages_pump(
                      trade_io_service, tmessage_received_delegete));
    trade_messages_pumps.push_back(tmessage_pump);
    tmessage_pump->start(trade_cp.first, trade_cp.second);
  }

  for (size_t i = 0; i < s.quote_connections.size(); ++i)
  {
    const connection_point& quote_cp = s.quote_connections[i];
    market_messages_pump_ptr qmessage_pump(new market_messages_pump(
                      quote_io_service, qmessage_received_delegete));
    quote_messages_pumps.push_back(qmessage_pump);
    qmessage_pump->start(quote_cp.first, quote_cp.second);
  }


  for (size_t i = 0; i < s.trade_thread_size; ++i)
    io_workers.create_thread(boost::bind(io_thread, boost::ref(trade_io_service)));

  for (size_t i = 0; i < s.quote_thread_size; ++i)
    io_workers.create_thread(boost::bind(io_thread, boost::ref(quote_io_service)));

  int key = 0x00;
  std::cout << "Press [Ctrl] + [C] to stop\n";
  while (key != 0x03)
    key = _getch();

  for (size_t i = 0; i < trade_messages_pumps.size(); ++i)
    trade_messages_pumps[i]->stop();

  for (size_t i = 0; i < quote_messages_pumps.size(); ++i)
    quote_messages_pumps[i]->stop();

  trade_io_service.stop();
  quote_io_service.stop();
  io_workers.join_all();

  save_results(market_data_processor);
  
  return 0;
}

bool multicast_communication::settings::read_connections(std::ifstream& ifs, 
                                 std::vector<connection_point>& connections)
{
  std::string line;
  size_t connections_count = 0;

  try {
    getline(ifs, line);
    connections_count = boost::lexical_cast<size_t>(line.c_str());
  } catch (const boost::bad_lexical_cast& e) {
    std::cerr << "Error in multicast_communication::settings::read_connections "
              << "Reason " << e.what() << std::endl;
    return false;
  }

  connections.clear();
  for (size_t i = 0; i < connections_count; ++i)
  {
    getline(ifs, line);
    std::string address, port;
    size_t pos = line.find(' ');
    if (pos == std::string::npos)
      pos = line.find('\t');
    if (pos != std::string::npos)
    {
      address = line.substr(0, pos);
      while (pos < line.size() && (line[pos] == ' '
        || line[pos] == '\t'))
      { 
        ++pos;
      }

      unsigned short p = 0;
      if (pos < line.size())
        port = line.substr(pos);
      try {
        p = boost::lexical_cast<unsigned short>(port.c_str());
      } catch (boost::bad_lexical_cast& e) {
        std::cerr << "Error in multicast_communication::settings::read_connections "
                  << "Reason " << e.what() << std::endl;
        return false;
      }

      connections.push_back(connection_point(address, p));      
    } // if (pos != std::string::npos)
  } // for (size_t i = 0; i < connections_count; ++i)

  return true;
}

void multicast_communication::io_thread(boost::asio::io_service& service)
{
  service.run();
}

bool multicast_communication::read_settings(settings& s)
{
  s.clear();

  static const char* kConfigFilename = BINARY_DIR "/config.ini";

  std::ifstream config_ifs(kConfigFilename);
  if (!config_ifs.is_open())
  {
    std::cerr << "Can't open configuration file: " 
	      << kConfigFilename << std::endl; 
    return false;
  }

  std::string line;

  try {
    getline(config_ifs, line);
    s.trade_thread_size = boost::lexical_cast<size_t>(line.c_str());
    getline(config_ifs, line);
    s.quote_thread_size = boost::lexical_cast<size_t>(line.c_str());
  } catch (const boost::bad_lexical_cast& e) {
    std::cerr << "Error in multicast_communication::read_settings "
              << "Reason " << e.what() << std::endl;
    return false;
  }

  if (!settings::read_connections(config_ifs, s.trade_connections))
    return false;

  return settings::read_connections(config_ifs, s.quote_connections);
}

void multicast_communication::save_results(market_data_processor_ptr& mdp)
{
  static const char* kOutFilename = "market_data.dat";
  std::ofstream ofs(kOutFilename);

  if (!ofs.good())
  {
    std::cerr << "Can't open file: " << kOutFilename << std::endl;
    return;
  }

  mdp->dump_messages(ofs);
}
