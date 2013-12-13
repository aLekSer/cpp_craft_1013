#include <boost/test/test_tools.hpp>
namespace text_test
{
	void read_config();
	void quote_trade_parse();
}
namespace market_data
{
	void data_test();

}
namespace async_udp
{
	void receiver_test();
	namespace tests_
	{
		void udp_listener_tests();
		void udp_writer_tests();
	}
}
namespace minute_calc
{
	void calculate();
}