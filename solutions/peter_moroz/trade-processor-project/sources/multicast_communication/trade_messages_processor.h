#ifndef _MULTICAST_COMMUNICATION_TRADE_MESSAGES_PROCESSOR_H_
#define _MULTICAST_COMMUNICATION_TRADE_MESSAGES_PROCESSOR_H_

#include "trade_message.h"

namespace multicast_communication
{

  // I've used 'Chain-Of-Responsibility' design pattern here 
  // To decide which kind of CQS-message we need to process
  class trade_messages_processor
  {
  public:
    explicit trade_messages_processor();
    virtual ~trade_messages_processor();

  protected:
    trade_messages_processor(bool skip_initialize);

  private:
    void add_element_to_chain(trade_messages_processor* next);

  public:
    void initialize();
    virtual trade_message_ptr parse_message(const std::string& msg);

  protected:
    virtual bool is_parseable(const std::string& msg) { return false; }

    trade_messages_processor* get_parser(const std::string& msg);
    virtual trade_message_ptr do_parsing(const std::string& msg)
    {
      return trade_message_ptr(new trade_message());
    }

  private:
    trade_messages_processor* next_;   
  };

  typedef boost::shared_ptr<trade_messages_processor> trade_messages_processor_ptr;


  class long_trade_messages_processor : public trade_messages_processor
  {
		enum FieldsLengthes
		{
			kMessageHeader 			                = 24,
			kSecuritySymbol 			              = 11,
			kTemporarySuffix			              = 1,
			kTestMessageInd     		            = 1,
			kTradeReportingFacilityId		        = 1,
			kPrimaryListingMarketParticipantId  = 1,
			kReserved1			 	                  = 1,
			kFinancialStatus		 	              = 1,
			kCurrencyInd		 	                  = 3,
			kHeldTradeInd     		 	            = 1,
			kInstrumentType		 	                = 1,
			kSellerSaleDays	 		                = 3,
			kSaleConditon 			                = 4,
			kTradeThroughExemptInd      	      = 1,
			kShortSaleRestrictionInd      	    = 1,
			kReserved2				                  = 1,
			kPriceDenominatorInd      		      = 1,
			kTradePrice				                  = 12,
			kTradeVolume			                  = 9,
			// enough for our purpose
		};

  private:
    long_trade_messages_processor() : trade_messages_processor(true) {}
    ~long_trade_messages_processor() {}

  private:
    virtual bool is_parseable(const std::string& msg);
    virtual trade_message_ptr do_parsing(const std::string& msg);

    friend class trade_messages_processor;
  };


  class short_trade_messages_processor : public trade_messages_processor
  {
		enum FieldsLengthes
		{
			kMessageHeader 		    = 24,
			kSecuritySymbol 		  = 3,
			kSaleCondition		    = 1,
			kTradeVolume 		      = 4,
			kPriceDenominatorInd	= 1,
			kTradePrice		 	      = 8,
			// enough for our purpose
		};

  private:
    short_trade_messages_processor() : trade_messages_processor(true) {}
    ~short_trade_messages_processor() {}

  private:
    virtual bool is_parseable(const std::string& msg);
    virtual trade_message_ptr do_parsing(const std::string& msg);

    friend class trade_messages_processor;
  };
}

#endif // _MULTICAST_COMMUNICATION_TRADE_MESSAGES_PROCESSOR_H_
