#ifndef _MULTICAST_COMMUNICATION_TRADE_MESSAGE_H_
#define _MULTICAST_COMMUNICATION_TRADE_MESSAGE_H_

#include <string>
#include <boost/shared_ptr.hpp>

namespace multicast_communication
{
  class trade_message
  {
  public:
    explicit trade_message();
    explicit trade_message(const std::string& security_symbol,
                           const double& price, const double& volume);

    std::string security_symbol() const;
    double price() const;
    double volume() const;

  private:
    std::string security_symbol_;
    double price_;
    double volume_;
  };

  typedef boost::shared_ptr< trade_message > trade_message_ptr;
}

#endif // _MULTICAST_COMMUNICATION_TRADE_MESSAGE_H_
