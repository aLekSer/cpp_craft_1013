#include <stdexcept>
#include <sstream>
#include <boost/lexical_cast.hpp>

#include "quote_messages_processor.h"

namespace mcast_comm = multicast_communication;

mcast_comm::quote_messages_processor::quote_messages_processor() 
  : next_(NULL)
{
  initialize();
}
mcast_comm::quote_messages_processor::~quote_messages_processor()
{
  delete next_;
}

mcast_comm::quote_messages_processor::quote_messages_processor(bool skip_initialize) 
  : next_(NULL)
{
  if (!skip_initialize)
    initialize();
}


void mcast_comm::quote_messages_processor::add_element_to_chain(
                      mcast_comm::quote_messages_processor* next)
{
  std::stringstream err_message;
  if (next == NULL)
  {
    err_message 
      << "Method: quote_messages_processor::add_element_to_chain "
      << " Error: NULL-pointer has been passed as argument.\n";
    throw std::invalid_argument(err_message.str());
  }

  if (next->next_ != NULL)
  {
    err_message 
      << "Method: quote_messages_processor::add_element_to_chain "
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
void mcast_comm::quote_messages_processor::initialize()
{
  add_element_to_chain(new long_quote_messages_processor());
  add_element_to_chain(new short_quote_messages_processor());
}
mcast_comm::quote_message_ptr 
    mcast_comm::quote_messages_processor::parse_message(const std::string& m)
{
  quote_messages_processor* parser = get_parser(m);
  if (parser != NULL)
    return parser->do_parsing(m);
  return quote_message_ptr();
}

mcast_comm::quote_messages_processor*
    mcast_comm::quote_messages_processor::get_parser(const std::string& msg)
{
  if (is_parseable(msg))
    return this;
  return next_ != NULL ? next_->get_parser(msg) : NULL;
}

bool mcast_comm::long_quote_messages_processor::is_parseable(const std::string& m)
{
  // m[0] contains CATEGORY, m[1] contains TYPE
  return (m[1] == 'B') ? (m[0] == 'B' || m[0] == 'E' || m[0] == 'L') : false;
}

mcast_comm::quote_message_ptr 
    mcast_comm::long_quote_messages_processor::do_parsing(
    const std::string& m)
{
	size_t offset = kMessageHeader;

	std::string security_symbol = m.substr(offset, kSecuritySymbol);

	offset += kSecuritySymbol 
         + kTemporarySuffix 
         + kTestMessageIndicator
         + kPrimaryListingMarketParticipantInd
         + kSIP_GeneratedMessageInd
         + kReserved1
         + kFinancialStatus
         + kCurrencyIndicator
         + kInstrumentType
         + kCancelCorrectionInd
         + kSettlementCondition
         + kMarketCondition
         + kQuoteCondition
         + kLULD_Indicator 
         + kRetailInterestInd
         + kBidPriceDenominatorInd;

	std::string b_price = m.substr(offset, kBidPrice);
	offset += kBidPrice;
	std::string b_volume = m.substr(offset, kBidSizeInUnitsOfTrade);
	offset += kBidSizeInUnitsOfTrade
         + kOfferPriceDenominatorInd;
	std::string o_price = m.substr(offset, kOfferPrice);
	offset += kOfferPrice;
	std::string o_volume = m.substr(offset, kOfferSizeInUnitsOfTrade);
	
  double bid_price = 0.0;
  double bid_volume = 0.0;
  double offer_price = 0.0;
  double offer_volume = 0.0;

  try {
    bid_price = boost::lexical_cast<double>(b_price.c_str());
    bid_volume = boost::lexical_cast<double>(b_volume.c_str());
    offer_price = boost::lexical_cast<double>(o_price.c_str());
    offer_volume = boost::lexical_cast<double>(o_volume.c_str());
  } catch (const boost::bad_lexical_cast&) {
    return quote_message_ptr();
  }

  quote_message_ptr qmessage_ptr(new quote_message(security_symbol,
                bid_price, bid_volume, offer_price, offer_volume));
  return qmessage_ptr;
}


bool mcast_comm::short_quote_messages_processor::is_parseable(const std::string& m)
{
  // m[0] contains CATEGORY, m[1] contains TYPE
  return (m[1] == 'D') ? (m[0] == 'E' || m[0] == 'L') : false;
}
mcast_comm::quote_message_ptr 
    mcast_comm::short_quote_messages_processor::do_parsing(
    const std::string& m)
{
	size_t offset = kMessageHeader;

	std::string security_symbol = m.substr(offset, kSecuritySymbol);

	offset += kSecuritySymbol 
		     + kQuoteCondition 
         + kLULD_Indicator 
         + kReserved1
         + kBidPriceDenominatorInd;
	std::string b_price = m.substr(offset, kBidShortPrice);
	offset += kBidShortPrice;
	std::string b_volume = m.substr(offset, kBidSizeUnitsOfTrade);
	offset += kBidSizeUnitsOfTrade
         + kReserved2Length
         + kOfferPriceDenominatorInd;
	std::string o_price = m.substr(offset, kOfferShortPrice);
	offset += kOfferShortPrice;
	std::string o_volume = m.substr(offset, kOfferSizeInUnitsOfTrade);
	
  double bid_price = 0.0;
  double bid_volume = 0.0;
  double offer_price = 0.0;
  double offer_volume = 0.0;

  try {
    bid_price = boost::lexical_cast<double>(b_price.c_str());
    bid_volume = boost::lexical_cast<double>(b_volume.c_str());
    offer_price = boost::lexical_cast<double>(o_price.c_str());
    offer_volume = boost::lexical_cast<double>(o_volume.c_str());
  } catch (const boost::bad_lexical_cast&) {
    return quote_message_ptr();
  }

  quote_message_ptr qmessage_ptr(new quote_message(security_symbol,
                bid_price, bid_volume, offer_price, offer_volume));
  return qmessage_ptr;
}
