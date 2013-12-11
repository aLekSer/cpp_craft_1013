#include <stdexcept>
#include <sstream>
#include <boost/lexical_cast.hpp>

#include "trade_messages_processor.h"

namespace mcast_comm = multicast_communication;

mcast_comm::trade_messages_processor::trade_messages_processor() 
  : next_(NULL)
{
  initialize();
}
mcast_comm::trade_messages_processor::~trade_messages_processor()
{
  delete next_;
}
mcast_comm::trade_messages_processor::trade_messages_processor(bool skip_initialize)
  : next_(NULL)
{
  if (!skip_initialize)
    initialize();
}

void mcast_comm::trade_messages_processor::add_element_to_chain(
                      mcast_comm::trade_messages_processor* next)
{
  std::stringstream err_message;
  if (next == NULL)
  {
    err_message 
      << "Method: trade_messages_processor::add_element_to_chain "
      << " Error: NULL-pointer has been passed as argument.\n";
    throw std::invalid_argument(err_message.str());
  }

  if (next->next_ != NULL)
  {
    err_message 
      << "Method: trade_messages_processor::add_element_to_chain "
      << " Error: Not terminated chain element has been passed.\n";
    throw std::invalid_argument(err_message.str());
  }

  if (next_ == NULL)
    next_ = next;
  else
    next_->add_element_to_chain(next);
}

// everyone, who want to add some extra handlers to chain
// need to change method initialize()
void mcast_comm::trade_messages_processor::initialize()
{
  add_element_to_chain(new long_trade_messages_processor());
  add_element_to_chain(new short_trade_messages_processor());
}
mcast_comm::trade_message_ptr 
    mcast_comm::trade_messages_processor::parse_message(const std::string& m)
{
  trade_messages_processor* parser = get_parser(m);
  if (parser != NULL)
    return parser->do_parsing(m);
  return trade_message_ptr();
}

mcast_comm::trade_messages_processor* 
    mcast_comm::trade_messages_processor::get_parser(const std::string& msg)
{
  if (is_parseable(msg))
    return this;
  return next_ != NULL ? next_->get_parser(msg) : NULL;
}

bool mcast_comm::long_trade_messages_processor::is_parseable(const std::string& m)
{
  // m[0] contains CATEGORY, m[1] contains TYPE
  return (m[1] == 'B') ? (m[0] == 'B' || m[0] == 'E' || m[0] == 'L') : false;
}

mcast_comm::trade_message_ptr
    mcast_comm::long_trade_messages_processor::do_parsing(
    const std::string& m)
{
	size_t offset = kMessageHeader;

	std::string security_symbol = m.substr(offset, kSecuritySymbol);

	offset += kSecuritySymbol 
         + kTemporarySuffix 
         + kTestMessageInd
         + kTradeReportingFacilityId
         + kPrimaryListingMarketParticipantId
         + kReserved1
         + kFinancialStatus
         + kCurrencyInd
         + kHeldTradeInd
         + kInstrumentType
         + kSellerSaleDays
         + kSaleConditon
         + kTradeThroughExemptInd
         + kShortSaleRestrictionInd
         + kReserved2 
         + kPriceDenominatorInd;

	std::string price = m.substr(offset, kTradePrice);
	offset += kTradePrice;
	std::string volume = m.substr(offset, kTradeVolume);
	
  double p = 0.0;
  double v = 0.0;
  
  try {
    p = boost::lexical_cast<double>(price.c_str());
    v = boost::lexical_cast<double>(volume.c_str());
  } catch (const boost::bad_lexical_cast&) {
    return trade_message_ptr();
  }

  trade_message_ptr tmessage_ptr(new trade_message(security_symbol, p, v));
  return tmessage_ptr;
}

bool mcast_comm::short_trade_messages_processor::is_parseable(const std::string& m)
{
  // m[0] contains CATEGORY, m[1] contains TYPE
  return (m[1] == 'I') ? (m[0] == 'E' || m[0] == 'L') : false;
}
mcast_comm::trade_message_ptr 
    mcast_comm::short_trade_messages_processor::do_parsing(
    const std::string& m)
{
	size_t offset = kMessageHeader;
	std::string security_symbol = m.substr(offset, kSecuritySymbol);

	offset += kSecuritySymbol 
         + kSaleCondition;
  std::string volume = m.substr(offset, kTradeVolume);

  offset += kTradeVolume
         + kPriceDenominatorInd;
  std::string price = m.substr(offset, kTradePrice);

  double p = 0.0;
  double v = 0.0;

  try {
    p = boost::lexical_cast<double>(price.c_str());
    v = boost::lexical_cast<double>(volume.c_str());
  } catch (const boost::bad_lexical_cast&) {
    return trade_message_ptr();
  }

  trade_message_ptr tmessage_ptr(new trade_message(security_symbol, p, v));
  return tmessage_ptr;
}
