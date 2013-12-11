#include "market_messages_pump.h"

namespace mcast_comm = multicast_communication;

void mcast_comm::parse_transmission_block(const std::string& in_block,
                                std::vector<std::string>& out_messages)
{
  const char SOH = 0x01;
  const char ETX = 0x03;
  const char US = 0x1F;

  size_t p0 = in_block.find(SOH);
  size_t p1 = 0;

  std::string message;

  out_messages.clear();
  
  while (p0 != std::string::npos)
  {
    ++p0;
    if (p0 >= in_block.length())
      break;
    p1 = in_block.find(US, p0);
    if (p1 == std::string::npos)      // last message in block
    {
      p1 = in_block.find(ETX, p0);
      if (p1 == std::string::npos)    // abnormal situation
        p1 = in_block.length();
    }
    message = in_block.substr(p0, p1 - p0);
    out_messages.push_back(message);
    p0 = p1;
  } // while (p0 != std::string::npos)
}

mcast_comm::market_messages_pump::market_messages_pump(
    boost::asio::io_service& io_service,
    const market_data_received_delegate_ptr& msg_recv_delegate)
  : started_(false), socket_(io_service), 
    msg_recv_delegate_(msg_recv_delegate)
{
}
mcast_comm::market_messages_pump::~market_messages_pump() {}

void mcast_comm::market_messages_pump::start(const std::string& addr,
                                             unsigned short port)
{
  if (started())
    return;
  started_ = true;

  using boost::asio::ip::udp;
  using boost::asio::ip::address;
  using boost::asio::ip::multicast::join_group;
  
  boost::asio::ip::udp::endpoint ep(address::from_string("0.0.0.0"), port);
  socket_.open(ep.protocol());
  socket_.set_option(udp::socket::reuse_address(true));
  socket_.bind(ep);
  socket_.set_option(join_group(address::from_string(addr)));

  receive();
}
void mcast_comm::market_messages_pump::stop()
{
  if (!started())
    return;
  started_ = false;
  socket_.close();
}

void mcast_comm::market_messages_pump::receive()
{
  using namespace boost::asio::placeholders;

  socket_.async_receive(boost::asio::buffer(recv_buffer_),
            boost::bind(&market_messages_pump::on_receive, 
            this, recv_buffer_, error, bytes_transferred));
}
void mcast_comm::market_messages_pump::on_receive(
    char* buffer, const sys_err_code& error, const size_t recv_bytes_count)
{
  if (error)
    return;
  transmission_block_.assign(recv_buffer_, recv_bytes_count);
  transfer_data();
  receive();
}

void mcast_comm::market_messages_pump::transfer_data()
{
  std::vector<std::string> messages;
  parse_transmission_block(transmission_block_, messages);

  // TO DO: it might be better - transfer all messages
  // in single method call.
  for (size_t i = 0; i < messages.size(); ++i)
    msg_recv_delegate_->on_message_received(messages[i]);
}
