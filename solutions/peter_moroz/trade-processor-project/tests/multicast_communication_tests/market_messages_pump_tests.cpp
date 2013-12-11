#include "test_registrator.h"
#include "test_detail.h"

#include <market_messages_pump.h>

namespace mcast_comm = multicast_communication;

namespace multicast_communication
{
	namespace tests_
	{
		namespace detail
		{
			void service_thread(boost::asio::io_service& service);

      // dummy market_receiver, which just accumulate received messages
      class test_market_data_receiver : public mcast_comm::message_received_events
      {
      public:
        test_market_data_receiver() {}

        const std::vector<std::string>& qmessages() const 
        {
          boost::unique_lock<boost::mutex> lock(q_guard_);
          return qmessages_;
        }
        const std::vector<std::string>& tmessages() const 
        {
          boost::unique_lock<boost::mutex> lock(t_guard_);
          return tmessages_;
        }

      public:
        virtual void on_quote_message(const std::string& msg)
        {
          boost::unique_lock<boost::mutex> lock(q_guard_);
          qmessages_.push_back(msg);
        }
        virtual void on_trade_message(const std::string& msg)
        {
          boost::unique_lock<boost::mutex> lock(t_guard_);
          tmessages_.push_back(msg);
        }

      private:
        mutable boost::mutex q_guard_;
        mutable boost::mutex t_guard_;
        std::vector<std::string> qmessages_;
        std::vector<std::string> tmessages_;
      }; // class test_market_data_receiver

      typedef boost::shared_ptr<test_market_data_receiver> test_market_data_receiver_ptr;

		} // namespace detail
	} // namespace tests_
} // namespace multicast_communication

void mcast_comm::tests_::detail::service_thread(boost::asio::io_service& service)
{
	service.run();
}

void mcast_comm::tests_::market_messages_pump_tests()
{
	using namespace boost::asio;
	static const size_t kMessagesCount = 1;
	io_service service;

  detail::test_market_data_receiver_ptr test_data_receiver_p;
  market_data_received_delegate_ptr qmessage_received_delegate_p;
  market_data_received_delegate_ptr tmessage_received_delegate_p;


  BOOST_CHECK_NO_THROW
  (
    test_data_receiver_p.reset(new detail::test_market_data_receiver());
  );

  BOOST_CHECK_NO_THROW
  (  
    qmessage_received_delegate_p.reset(new qmessage_received_delegate(test_data_receiver_p));
  );

  BOOST_CHECK_NO_THROW
  (  
    tmessage_received_delegate_p.reset(new tmessage_received_delegate(test_data_receiver_p));
  );

  // test quotas delivering channel
  {
    const char* kDataDir = SOURCE_DIR "/tests/data/";
    const std::string addr = "233.200.79.9";
    const unsigned short port = 61009;

    std::string fname = kDataDir;
    fname.append(addr);
    fname.append(".udp");
    std::ifstream datasource(fname.c_str(), std::ios::binary);
    BOOST_CHECK(datasource.is_open());

    std::vector<std::string> datablocks;
    detail::parse_datasource(datasource, datablocks);

		market_messages_pump mm_pump(service, qmessage_received_delegate_p);
		mm_pump.start(addr, port);

		ip::udp::endpoint ep(ip::address::from_string(addr), port);
		ip::udp::socket sock(service, ep.protocol());
		boost::thread worker(boost::bind(detail::service_thread, boost::ref(service)));

    for (size_t i = 0; i < datablocks.size(); ++i)
		{
      const std::string& data = datablocks[i];
			sock.send_to(buffer(data.c_str(), data.size()), ep);
      // I'm still using boost 1.49, so no any chrono ...
      boost::this_thread::sleep(boost::posix_time::microseconds(1));
		}

		service.stop();
		worker.join();
    mm_pump.stop();

    std::vector<std::string> original_messages;
    for (size_t i = 0; i < datablocks.size(); ++i)
    {
      std::vector<std::string> messages;
      parse_transmission_block(datablocks[i], messages);
      for (size_t j = 0; j < messages.size(); ++j)
        original_messages.push_back(messages[j]);
    }

    const std::vector<std::string>& messages = test_data_receiver_p->qmessages();
    // pull out your network plug from socket, because 
    // if you haven't done so, you will get some extra 
    // messages from network and test will be failed
    for (size_t i = 0; i < messages.size(); ++i)
    {
      BOOST_CHECK(std::find(original_messages.begin(),
            original_messages.end(), messages[i]) != original_messages.end());
    }
  }

  // test trades delivering channel
  {
    const char* kDataDir = SOURCE_DIR "/tests/data/";
    const std::string addr = "233.200.79.139";
    const unsigned short port = 61139;
    std::string fname = kDataDir;
    fname.append(addr);
    fname.append(".udp");
    std::ifstream datasource(fname.c_str(), std::ios::binary);
    BOOST_CHECK(datasource.is_open());

    std::vector<std::string> datablocks;
    detail::parse_datasource(datasource, datablocks);

		market_messages_pump mm_pump(service, tmessage_received_delegate_p);
		mm_pump.start(addr, port);

		ip::udp::endpoint ep(ip::address::from_string(addr), port);
		ip::udp::socket sock(service, ep.protocol());
		boost::thread worker(boost::bind(detail::service_thread, boost::ref(service)));

    for (size_t i = 0; i < datablocks.size(); ++i)
		{
      const std::string& data = datablocks[i];
			sock.send_to(buffer(data.c_str(), data.size()), ep);
      // I'm still using boost 1.49, so no any chrono ...
      boost::this_thread::sleep(boost::posix_time::microseconds(1));
		}

		service.stop();
		worker.join();
    mm_pump.stop();

    std::vector<std::string> original_messages;
    for (size_t i = 0; i < datablocks.size(); ++i)
    {
      std::vector<std::string> messages;
      parse_transmission_block(datablocks[i], messages);
      for (size_t j = 0; j < messages.size(); ++j)
        original_messages.push_back(messages[j]);
    }

    const std::vector<std::string>& messages = test_data_receiver_p->tmessages();
    // pull out your network plug from socket, because 
    // if you haven't done so, you will get some extra 
    // messages from network and test will be failed
    for (size_t i = 0; i < messages.size(); ++i)
    {
      BOOST_CHECK(std::find(original_messages.begin(),
            original_messages.end(), messages[i]) != original_messages.end());
    }
  }
}
