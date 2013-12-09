#include "test_registrator.h"
#include <string>
#include "config.h"

#include <udp_listener.h>

#include <cstring>

void text_test::read_config()
{
	BOOST_CHECK_NO_THROW
	(
		config c(data_path + string("config.ini"));
	);
	{
		config c(data_path + string("config.ini"));
		BOOST_CHECK_EQUAL (c.get_trades().size(), c.trade_ports());
		BOOST_CHECK_EQUAL (c.get_quotes()[1].first, "233.200.79.1");
		ofstream o (string(data_path + string("out")).c_str());
		o<<c.get_quotes()[0].first;
		BOOST_CHECK_EQUAL (c.get_quotes().size(), c.quote_ports());
		o.close();
	}
}