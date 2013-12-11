#ifndef _MARKET_DATA_RECEIVER_MARKET_DATA_RECEIVER_H_
#define _MARKET_DATA_RECEIVER_MARKET_DATA_RECEIVER_H_

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include "../multicast_communication/market_data_processor.h"
#include "../multicast_communication/quote_messages_processor.h"
#include "../multicast_communication/trade_messages_processor.h"

namespace multicast_communication
{
  // interface-class. all entities, which should be 
  // notified about these events must implement it.
  class message_received_events
  {
  public:
    virtual void on_quote_message(const std::string& msg) = 0;
    virtual void on_trade_message(const std::string& msg) = 0;
  };

  typedef boost::shared_ptr<message_received_events> message_received_events_ptr;


  // I have to make all implementation of given class inside header-file,
  // because, if I did it inside usual cpp-file, I've got error messages,
  // when linker try to resolve names, which aren't belong to 
  // multicast_communication-library. So, main application was 
  // built succesfully, but building of test environment wasn't so nice.
  class market_data_receiver : public message_received_events
  {
  public:
    market_data_receiver(market_data_processor_ptr& market_data_processor_p)
      : market_data_processor_(market_data_processor_p),
        quote_messages_processor_(new quote_messages_processor()),
        trade_messages_processor_(new trade_messages_processor())
    {
    }

  public:
    virtual void on_quote_message(const std::string& msg)
    {
      quote_message_ptr qmessage = quote_messages_processor_->parse_message(msg);
      if (qmessage != NULL)
        market_data_processor_->new_quote(qmessage);
    }
    virtual void on_trade_message(const std::string& msg)
    {
      trade_message_ptr tmessage = trade_messages_processor_->parse_message(msg);
      if (tmessage != NULL)
        market_data_processor_->new_trade(tmessage);
    }
  
  private:
    market_data_processor_ptr market_data_processor_;
    boost::scoped_ptr<quote_messages_processor> quote_messages_processor_;
    boost::scoped_ptr<trade_messages_processor> trade_messages_processor_;
  };

  typedef boost::shared_ptr<market_data_receiver> market_data_receiver_ptr;


  class market_data_received_delegate
  {
  protected:
    market_data_received_delegate(const message_received_events_ptr& events_handler) 
      : events_handler_(events_handler) {}

  public:
    virtual void on_message_received(const std::string& msg) = 0;
  protected:
    message_received_events_ptr events_handler_;
  };

  typedef boost::shared_ptr<market_data_received_delegate> market_data_received_delegate_ptr;


  class qmessage_received_delegate : public market_data_received_delegate
  {
  public:
    qmessage_received_delegate(const message_received_events_ptr& events_handler) 
      : market_data_received_delegate(events_handler) {}
      
  public:
    virtual void on_message_received(const std::string& msg)
    {
      events_handler_->on_quote_message(msg);
    }
  };

  class tmessage_received_delegate : public market_data_received_delegate
  {
  public:
    tmessage_received_delegate(const message_received_events_ptr& events_handler) 
      : market_data_received_delegate(events_handler) {}
      
  public:
    virtual void on_message_received(const std::string& msg)
    {
      events_handler_->on_trade_message(msg);
    }
  };

}

#endif // _MARKET_DATA_RECEIVER_DATA_RECEIVER_H_