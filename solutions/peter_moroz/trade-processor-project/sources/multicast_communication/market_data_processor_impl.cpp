#include <fstream>
#include <iomanip>
#include <iostream>
#include "market_data_processor.h"

namespace mcast_comm = multicast_communication;

size_t mcast_comm::market_data_processor_impl::tmessages_count() const
{
  boost::unique_lock<boost::mutex> lock(tmsg_guard_);
  return tmessages_.size();
}
size_t mcast_comm::market_data_processor_impl::qmessages_count() const
{
  boost::unique_lock<boost::mutex> lock(qmsg_guard_);
  return qmessages_.size();
}
void mcast_comm::market_data_processor_impl::withdraw_trades(
                 std::vector<const trade_message_ptr>& trades)
{
  boost::unique_lock<boost::mutex> lock(tmsg_guard_);
  trades.clear();
  trades.reserve(tmessages_.size());
  while (tmessages_.size())
  {
    trades.push_back(tmessages_.front());
    tmessages_.pop_front();    
  }
}
void mcast_comm::market_data_processor_impl::withdraw_quotas(
                 std::vector<const quote_message_ptr>& quotas)
{
  boost::unique_lock<boost::mutex> lock(qmsg_guard_);
  quotas.clear();
  quotas.reserve(qmessages_.size());
  while (qmessages_.size())  
  {
    quotas.push_back(qmessages_.front());
    qmessages_.pop_front();
  }
}


void print_message(const mcast_comm::trade_message_ptr& message, std::ostream& os)
{
  // assume, that output stream is STDOUT or text-file
  // use standart manipulators and insertion operators
  os << 'T' << message->security_symbol() << ' ' 
    << std::setiosflags(std::ios::fixed) << std::setw(4) << std::setfill('0')
    << std::setprecision(2) << message->price() << ' '
    << std::setprecision(1) << message->volume() << std::endl;
}
void print_message(const mcast_comm::quote_message_ptr& message, std::ostream& os)
{
  // assume, that output stream is STDOUT or text-file
  // use standart manipulators and insertion operators
  os << 'Q' << message->security_symbol() << ' ' 
    << std::setiosflags(std::ios::fixed) << std::setw(4) << std::setfill('0')
    << std::setprecision(2) << message->bid_price() << ' '
    << std::setprecision(1) << message->bid_volume() << ' '
    << std::setprecision(2) << message->offer_price() << ' ' 
    << std::setprecision(1) << message->offer_volume() << std::endl;
}

void mcast_comm::market_data_processor_impl::dump_messages(std::ofstream& dst)
{
  {
    boost::unique_lock<boost::mutex> lock(tmsg_guard_);
    while (tmessages_.size())
    {
      const trade_message_ptr& message = tmessages_.front();
      print_message(message, dst);
      tmessages_.pop_front();
    }
  }

  {
    boost::unique_lock<boost::mutex> lock(qmsg_guard_);
    while (qmessages_.size())
    {
      const quote_message_ptr& message = qmessages_.front();
      print_message(message, dst);
      qmessages_.pop_front();
    }
  }
}

void mcast_comm::market_data_processor_impl::new_trade(
       const mcast_comm::trade_message_ptr& message_ptr)
{
  boost::unique_lock<boost::mutex> lock(tmsg_guard_);
  tmessages_.push_back(message_ptr);
}
void mcast_comm::market_data_processor_impl::new_quote(
       const mcast_comm::quote_message_ptr& message_ptr)
{
  boost::unique_lock<boost::mutex> lock(qmsg_guard_);
  qmessages_.push_back(message_ptr);
}
