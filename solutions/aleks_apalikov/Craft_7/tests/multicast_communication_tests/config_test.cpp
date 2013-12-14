#include "test_registrator.h"
#include <string>
#include "config.h"

#include <udp_listener.h>

#include <cstring>

// full configuration test with some amount of quote and trade listeners
void text_test::read_config() 
{

	BOOST_CHECK_NO_THROW
	(
		config c(data_path + string("config.ini"));
	);
	{
		config c(data_path + string("config.ini"));
		BOOST_CHECK_EQUAL (c.get_trades().size(), c.trade_ports());
		if(c.get_quotes().size() > 1)
			BOOST_CHECK_EQUAL (c.get_quotes()[1].first, "233.200.79.1");
		ofstream o (string(data_path + string("out")).c_str());
		if(c.get_quotes().size() >= 1)
			o<<c.get_quotes()[0].first;
		else
			BOOST_CHECK_EQUAL (c.get_quotes().size() != 0, true);
		BOOST_CHECK_EQUAL (c.get_quotes().size(), c.quote_ports());
		o.close();
	}
}