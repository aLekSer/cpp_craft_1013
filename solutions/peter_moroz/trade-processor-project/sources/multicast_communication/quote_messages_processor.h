#ifndef _MULTICAST_COMMUNICATION_QUOTE_MESSAGES_PROCESSOR_H_
#define _MULTICAST_COMMUNICATION_QUOTE_MESSAGES_PROCESSOR_H_

#include "quote_message.h"

namespace multicast_communication
{

  // I've used 'Chain-Of-Responsibility' design pattern here 
  // To decide which kind of CQS-message we need to process
  class quote_messages_processor
  {
  public:
    explicit quote_messages_processor();
    virtual ~quote_messages_processor();

  protected:
    quote_messages_processor(bool skip_initialize);

  private:
    void add_element_to_chain(quote_messages_processor* next);

  public:
    void initialize();
    quote_message_ptr parse_message(const std::string& msg);

  protected:
    virtual bool is_parseable(const std::string& msg) { return false; }

    quote_messages_processor* get_parser(const std::string& msg);
    virtual quote_message_ptr do_parsing(const std::string& msg)
    {
      return quote_message_ptr();
    }

  private:
    quote_messages_processor* next_;
  };

  typedef boost::shared_ptr<quote_messages_processor> quote_messages_processor_ptr;


  class long_quote_messages_processor : public quote_messages_processor
  {
    enum FieldsLengthes
    {
			kMessageHeader 			                = 24,
			kSecuritySymbol 			              = 11,
			kTemporarySuffix			              = 1,
			kTestMessageIndicator		            = 1,
			kPrimaryListingMarketParticipantInd	= 1,
			kSIP_GeneratedMessageInd		        = 1,
			kReserved1			 	                  = 1,
			kFinancialStatus		 	              = 1,
			kCurrencyIndicator		 	            = 3,
			kInstrumentType		 	                = 1,
			kCancelCorrectionInd 		            = 1,
			kSettlementCondition		            = 1,
			kMarketCondition			              = 1,
			kQuoteCondition			                = 1,
			kLULD_Indicator			                = 1,
			kRetailInterestInd			            = 1,
			kBidPriceDenominatorInd             = 1,
			kBidPrice				                    = 12,
			kBidSizeInUnitsOfTrade		          = 7,
			kOfferPriceDenominatorInd           = 1,
			kOfferPrice				                  = 12,
			kOfferSizeInUnitsOfTrade		        = 7,
			// enough for our purpose
    };

  private:
    long_quote_messages_processor() : quote_messages_processor(true) {}
    ~long_quote_messages_processor() {}

  private:
    virtual bool is_parseable(const std::string& msg);
    virtual quote_message_ptr do_parsing(const std::string& msg);

    friend class quote_messages_processor;
  };


  class short_quote_messages_processor : public quote_messages_processor
  {
		enum FieldsLengthes
		{
			kMessageHeader 		        = 24,
			kSecuritySymbol 		      = 3,
			kQuoteCondition		        = 1,
			kLULD_Indicator 		      = 1,
			kReserved1 			          = 1,
			kBidPriceDenominatorInd	  = 1,
			kBidShortPrice	 	        = 8,
			kBidSizeUnitsOfTrade 	    = 3,
			kReserved2Length 		      = 1,
			kOfferPriceDenominatorInd	= 1,
			kOfferShortPrice		      = 8,
			kOfferSizeInUnitsOfTrade	= 3,
			// enough for our purpose
		};

  private:
    short_quote_messages_processor() : quote_messages_processor(true) {}
    ~short_quote_messages_processor() {}

  private:
    virtual bool is_parseable(const std::string& msg);
    virtual quote_message_ptr do_parsing(const std::string& msg);

    friend class quote_messages_processor;
  };
}

#endif // _MULTICAST_COMMUNICATION_QUOTE_MESSAGES_PROCESSOR_H_
