#include "test_registrator.h"
#include "test_detail.h"

#include <quote_messages_processor.h>
// only for standalone function 'parse_transmission_block'
#include <market_messages_pump.h>


void multicast_communication::tests_::quote_messages_processor_tests()
{
	BOOST_CHECK_NO_THROW
	( 
		quote_messages_processor qm_processor;
	)

	{
		quote_messages_processor qm_processor;
    const char* kDataDir = SOURCE_DIR "/tests/data/";
    const std::string addr = "233.200.79.9";

    std::string fname = kDataDir;
    fname.append(addr);
    fname.append(".udp");
    std::ifstream datasource(fname.c_str(), std::ios::binary);
    BOOST_CHECK(datasource.is_open());

    std::vector<std::string> datablocks;
    detail::parse_datasource(datasource, datablocks);

    std::vector<std::string> original_messages;
    for (size_t i = 0; i < datablocks.size(); ++i)
    {
      std::vector<std::string> messages;
      parse_transmission_block(datablocks[i], messages);
      for (size_t j = 0; j < messages.size(); ++j)
        original_messages.push_back(messages[j]);
    }

    for (size_t i = 0; i < original_messages.size(); ++i)
    {
      const std::string& msg = original_messages[i];
      char msg_category = msg[0];
      char msg_type = msg[1];
      if (msg_type == 'B')
      {
        // long quote messages
        if (msg_category == 'B' || msg_category == 'E' || msg_category == 'L')
        {
          quote_message_ptr qmp = qm_processor.parse_message(msg);
          BOOST_CHECK(qmp.get() != NULL);
        }
      }
      else if (msg_type == 'D')
      {
        // short quote messages
        if (msg_category == 'E' || msg_category == 'L')
        {
          quote_message_ptr qmp = qm_processor.parse_message(msg);
          BOOST_CHECK(qmp.get() != NULL);
        }
      }
    }
	}
}
