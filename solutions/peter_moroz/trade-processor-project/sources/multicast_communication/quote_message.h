#ifndef _MULTICAST_COMMUNICATION_QUOTE_MESSAGE_H_
#define _MULTICAST_COMMUNICATION_QUOTE_MESSAGE_H_

#include <boost/shared_ptr.hpp>

namespace multicast_communication
{
  class quote_message
  {
  public:
    explicit quote_message();
    explicit quote_message(const std::string& security_symbol,
                           const double& bid_price, const double& bid_volume,
                           const double& offer_price, const double& offer_volume);

    std::string security_symbol() const;
    double bid_price() const;
    double bid_volume() const;
    double offer_price() const;
    double offer_volume() const;

  private:
    std::string security_symbol_;
    double bid_price_;
    double bid_volume_;
    double offer_price_;
    double offer_volume_;
  };

  typedef boost::shared_ptr< quote_message > quote_message_ptr;
}

#endif // _MULTICAST_COMMUNICATION_QUOTE_MESSAGE_H_
