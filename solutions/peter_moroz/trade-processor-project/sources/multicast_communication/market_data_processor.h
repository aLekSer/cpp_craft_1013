#ifndef _MULTICAST_COMMUNICATION_MARKET_DATA_PROCESSOR_H_
#define _MULTICAST_COMMUNICATION_MARKET_DATA_PROCESSOR_H_

#include <fstream>
#include <list>
#include <boost/thread.hpp>

#include "trade_message.h"
#include "quote_message.h"


namespace multicast_communication
{
  class market_data_processor
  {
    friend class market_data_receiver; // please insert here your processing class 
  public:
    explicit market_data_processor(){}
    virtual ~market_data_processor(){}

    // In fact, there are no need in these
    // methods, only for test environment.
    virtual size_t tmessages_count() const { return 0; }
    virtual size_t qmessages_count() const { return 0; }
    virtual void withdraw_trades(std::vector<const trade_message_ptr>& trades) {}
    virtual void withdraw_quotas(std::vector<const quote_message_ptr>& quotas) {}

    virtual void dump_messages(std::ofstream& dst) {}

  private:
    virtual void new_trade( const trade_message_ptr& ) = 0;
    virtual void new_quote( const quote_message_ptr& ) = 0;
  };

  typedef boost::shared_ptr<market_data_processor> market_data_processor_ptr;

  class market_data_processor_impl : public market_data_processor
  {
  public:
    explicit market_data_processor_impl(){}
    virtual ~market_data_processor_impl(){}

    virtual size_t tmessages_count() const;
    virtual size_t qmessages_count() const;
    virtual void withdraw_trades(std::vector<const trade_message_ptr>& trades);
    virtual void withdraw_quotas(std::vector<const quote_message_ptr>& quotas);

    virtual void dump_messages(std::ofstream& dst);

  private:
    virtual void new_trade( const trade_message_ptr& );
    virtual void new_quote( const quote_message_ptr& );

  private:
    mutable boost::mutex tmsg_guard_;
    mutable boost::mutex qmsg_guard_;
    std::list<const trade_message_ptr> tmessages_;
    std::list<const quote_message_ptr> qmessages_;
  };
}

#endif // _MULTICAST_COMMUNICATION_MARKET_DATA_PROCESSOR_H_
