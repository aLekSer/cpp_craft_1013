#include "test_registrator.h"
#include "../../sources/multicast_communication/message_parser.h"
#include "../../sources/multicast_communication/config.h"
#include "minute_calculator.h"

void minute_calc::calculate()
{
/*	{
		stringstream ss;
		ss << "EBEO A  003759557N:J_735AVB             0    AAAR B30000012127000000001D0000012137000000001     A   62TB00012130001 BB00012137001 "
			<<"EDEO A  003759121P:J_428AINR  D00352000001 F00354300001 02" + string("\x1f")
			<<"LDEO A  003759122N:J`432ALJR  F00124900003 D00125100001 02";
		boost::shared_ptr<quote> q;
		using namespace boost;
		vector_messages msgs;
		vector_messages::iterator it;
		boost::shared_ptr<string> shar_str ( new string( ss.str()) );
		message::divide_messages(msgs, shar_str, true);	
		BOOST_CHECK_NO_THROW (
			it = msgs.begin();
		)
		q = boost::static_pointer_cast<quote, message> (*it) ;
		minute_calculator mc;
		mc.push_quote(q);
		if(it != msgs.end())
			it++;
		q = boost::static_pointer_cast<quote, message> (*it) ;
		mc.push_quote(q);
		if(it != msgs.end())
			it++;
		q = boost::static_pointer_cast<quote, message> (*it) ;
		mc.push_quote(q);

	}*/
	{
		stringstream ss;
		ss << "EBAO A  000146234N:3]004ACN             0     000 F  1  D000000779000000000100DD 0"
			<< "EIAO A  000146235T:3]008ACN@0100B00007790DD EIAO A  000146236K:3]009ACN@0100F77900000DD "
			<< "EIAO A  000146237T:3]010ACN@0100B00007790DD "
			<< "EBAO A  000146238D:3]010ABEV         T  0     000@   0  C000000007625000000100DD 0"
			<< "EBAO A  000146239D:3]034ABEV         T  0     000@   0  C000000007629000000400DD 0"
			<< "EIAO A  000146240P:3^207A  @0300D00549500DD EIAO A  000146241N:3]208A  @0700D00549500DD "
			<< "EIAO A  000146242T:3^208AON@0100B00008095DD EIAO A  000146243T:3]208AON@0100B00008095DD "
			<< "EBAO A  000146244N:3^209AON             0     000 F  1  D000000809500000000100DD 0"
			<< "EBAO A  000146245P:3_210A               0     000 F  1  D000000549500000000100DD 0"
			<< "EBAO A  000146246T:3_210A               0     000 F  1  B000000005495000000300DD 0"
			<< "EBAO A  000146247Z:3_211A               0     000 F  1  B000000005495000000100DD 0";


		boost::shared_ptr<trade> q;
		using namespace boost;
		vector_messages msgs;
		vector_messages::iterator it;
		boost::shared_ptr<string> shar_str ( new string( ss.str()) );
		message::divide_messages(msgs, shar_str, false);	
		BOOST_CHECK_NO_THROW (
			it = msgs.begin();
		)
		minute_calculator mc(new thread_safe_queue<shared_map>);
		for(int i = 0; i < 14; i++)
		{
			q = boost::static_pointer_cast<trade, message> (*it) ;
			mc.push_trade(q);
			if(it != msgs.end())
				it++;
		}
		BOOST_CHECK_EQUAL(mc.get_stat()->find("ACN")->first, "ACN");
	}
}
