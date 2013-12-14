#include "test_registrator.h"
#include "../../sources/multicast_communication/message_parser.h"
#include "../../sources/multicast_communication/config.h"
#include "minute_calculator.h"

void minute_calc::calculate()
{
	{
		stringstream ss;
		ss <<"EBAO A  000146234N:3]004ACN             0     000 F  1  D000000779000000000100DD 0""\x03"
			<< "EIAO A  000146235T:3]008ACN@0100I00002000DD""\x03"
			<< "EIAO A  000146237T:3]010ACN@0100I00001000DD""\x03"
			<< "EBAO A  000146238D:3]010ABEV         T  0     000@   0  C000000007625000000100DD 0""\x03"
			<< "EBAO A  000146239D:3]034ABEV         T  0     000@   0  C000000007629000000400DD 0""\x03"
			<< "EIAO A  000146240P:3^207A  @0300D00549500DD""\x03"
			<< "EIAO A  000146242T:3^208AON@0100B00000500DD""\x03"
			<< "EIAO A  000146237T:5]010ACN@0100I00001000DD""\x03"
			<< "EBAO A  000146244N:3^209AON             0     000 F  1  D000000809500000000100DD 0""\x03"
			<< "EBAO A  000146245P:4_210A               0     000 F  1  D000000549500000000100DD 0""\x03";
			//"\x03"


		boost::shared_ptr<trade> q;
		using namespace boost;
		vector_messages msgs;
		vector_messages::iterator it;
		boost::shared_ptr<string> shar_str ( new string( ss.str()) );
		message::divide_messages(msgs, shar_str, false);	
		BOOST_CHECK_NO_THROW (
			it = msgs.begin();
		)
		thread_safe_queue<shared_map>* que = new thread_safe_queue<shared_map>;
		minute_calculator mc(que);
		q = boost::static_pointer_cast<trade, message> (*it) ;
		for(int i = 0; i < 9; i++)
		{
			q = boost::static_pointer_cast<trade, message> (*it) ;
			mc.push_trade(q);
			if(i == 6)
			{
				shared_map sh_map;
				mc.tell_data(sh_map);
				BOOST_CHECK_EQUAL((*sh_map)["ACN"]->volume, 200.0);
				BOOST_CHECK_EQUAL((*sh_map)["ACN"]->open_price, 2000.0);
				BOOST_CHECK_EQUAL((*sh_map)["ACN"]->close_price, 1000.0);
				BOOST_CHECK_EQUAL((*sh_map)["ACN"]->high_price, 2000.0);
				BOOST_CHECK_EQUAL((*sh_map)["ACN"]->low_price, 1000.0);
				BOOST_CHECK_EQUAL((*sh_map)["AON"]->low_price, 5.0);
				BOOST_CHECK_EQUAL((*sh_map)["AON"]->high_price, 5.0);
			}

			q.reset();
			if(it != msgs.end())
			{
				it->reset();
				it++;
			}
		}
		shared_map sh_map;
		mc.tell_data(sh_map);
		BOOST_CHECK_EQUAL((*sh_map)["ACN"]->volume, 100.0);
		while (!que->empty())
		{
			shared_map sm;
			que->pop(sm);
			sm.reset();
		}
		delete que;
		BOOST_CHECK_EQUAL(mc.get_stat()->find("ACN")->first, "ACN");
		ss.str("");
	}
}
