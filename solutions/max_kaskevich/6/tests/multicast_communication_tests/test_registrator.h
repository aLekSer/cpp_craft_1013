#include <boost/test/test_tools.hpp>

namespace multicast_communication
{
	namespace tests_
	{
		void quote_message_tests();
		void trade_message_tests();

		void market_data_processor_tests();
        void udp_listener_tests();
        void thread_safe_queue_tests();
        void thread_safe_queue_many_thread_tests();
        void thread_safe_queue_a_lot_of_thread_tests();
	}
}
