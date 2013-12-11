#include "quote_message.h"

namespace mcast_comm = multicast_communication;

mcast_comm::quote_message::quote_message() : security_symbol_(""), 
  bid_price_(0.0), bid_volume_(0.0), offer_price_(0.0), offer_volume_(0.0) {}

mcast_comm::quote_message::quote_message(const std::string& security_symbol,
    const double& bid_price, const double& bid_volume,
    const double& offer_price, const double& offer_volume) 
    : security_symbol_(security_symbol), 
      bid_price_(bid_price), bid_volume_(bid_volume),
      offer_price_(offer_price), offer_volume_(offer_volume) {}

std::string mcast_comm::quote_message::security_symbol() const
{
  return security_symbol_;
}

double mcast_comm::quote_message::bid_price() const
{
  return bid_price_;
}

double mcast_comm::quote_message::bid_volume() const
{
  return bid_volume_;
}

double mcast_comm::quote_message::offer_price() const
{
  return offer_price_;
}

double mcast_comm::quote_message::offer_volume() const
{
  return offer_volume_;
}
