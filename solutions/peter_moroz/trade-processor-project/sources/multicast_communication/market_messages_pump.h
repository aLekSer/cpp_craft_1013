#ifndef _MULTICAST_COMMUNICATION_MARKET_MESSAGES_PUMP_H_
#define _MULTICAST_COMMUNICATION_MARKET_MESSAGES_PUMP_H_

#include <string>
#include <boost/asio.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "../market_data_receiver/market_data_receiver.h"

namespace multicast_communication
{
  typedef std::pair<std::string, unsigned int> connection_point;

  // split transmission block into messages
  void parse_transmission_block(const std::string& in_block,
                                 std::vector<std::string>& out_messages);

  // class responsible for receiving data from network
  class market_messages_pump
  {
    static const size_t kRecvBufferSize = 1000;
    typedef boost::system::error_code sys_err_code;

  public:
    explicit market_messages_pump(boost::asio::io_service& io_service,
              const market_data_received_delegate_ptr& msg_recv_delegate);
    virtual ~market_messages_pump();

  public:
    void start(const std::string& addr, unsigned short port);
    void stop();
    bool started() const { return started_; }

  private:
    void receive();
    void on_receive(char* buffer, const sys_err_code& error, 
                    const size_t recv_bytes_count);

    void transfer_data();

  private:
    bool started_;
    boost::asio::ip::udp::socket socket_;
    market_data_received_delegate_ptr msg_recv_delegate_;
    char recv_buffer_[kRecvBufferSize];
    std::string transmission_block_;
  };

  typedef boost::shared_ptr<market_messages_pump> market_messages_pump_ptr;
}

#endif // _MULTICAST_COMMUNICATION_MARKET_MESSAGES_PUMP_H_
