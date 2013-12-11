#include "trade_message.h"

namespace mcast_comm = multicast_communication;

mcast_comm::trade_message::trade_message()
  : security_symbol_(""), price_(0.0), volume_(0.0) {}

mcast_comm::trade_message::trade_message(
    const std::string& security_symbol,
    const double& price, const double& volume)
  : security_symbol_(security_symbol), price_(price), volume_(volume) {}


std::string mcast_comm::trade_message::security_symbol() const
{
  return security_symbol_;
}

double mcast_comm::trade_message::price() const
{
  return price_;
}

double mcast_comm::trade_message::volume() const
{
  return volume_;
}
