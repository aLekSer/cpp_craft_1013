#include "test_registrator.h"
#include <string>
#include "message_parser.h"
#include "config.h"
#include <sstream>
#include "market_data_processor.h"
#include "boost/thread.hpp"
#include "boost/shared_ptr.hpp"

void text_test::quote_trade_parse()
{
	{
		stringstream ss;
		string str = data_path + string("1.udp");
		ofstream ofs; 
		ofs.open(str.c_str());
		market_data_processor processor("3msgs_parser_test.txt");
		ss << "EBEO A  003759557N:J_735AVB             0    AAAR B30000012127000000001D0000012137000000001     A   62TB00012130001 BB00012137001 "
			<<"EDEO A  003759121P:J_428AINR  D00352000001 F00354300001 02" + string("\x1f")
			<<"LDEO A  003759122N:J_432ALJR  F00124900003 D00125100001 02";
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
		BOOST_CHECK_EQUAL(q->bid_denom(), '3');
		BOOST_CHECK_EQUAL(q->bid_price(), 1212700.0);
		BOOST_CHECK_EQUAL(q->bid_volume(), 1);
		BOOST_CHECK_EQUAL(q->offer_denom(), 'D');
		BOOST_CHECK_EQUAL(q->offer_price(), 1213700.0);
		BOOST_CHECK_EQUAL(q->offer_volume(), 1);
		BOOST_CHECK_EQUAL((char)q->get_categ(), 'E');
		BOOST_CHECK_EQUAL(q->msec(), (10 * 3600 + 26 * 60 + 47) * 1000 + 735);
		processor.wr_quote( q );

		it++;
		q = boost::static_pointer_cast<quote, message> (*it);
		BOOST_CHECK_EQUAL(q->bid_denom(), 'D');
		BOOST_CHECK_EQUAL(q->bid_price(), 352000.0);
		BOOST_CHECK_EQUAL(q->bid_volume(), 1);
		BOOST_CHECK_EQUAL(q->offer_denom(), 'F');
		BOOST_CHECK_EQUAL(q->offer_price(), 354300.0);
		BOOST_CHECK_EQUAL(q->offer_volume(), 1);
		BOOST_CHECK_EQUAL((char)q->get_type(), 'D');
		processor.wr_quote( q );

		if(it != msgs.end())
			it++;
		q = boost::static_pointer_cast<quote, message> (*it) ;
		BOOST_CHECK_EQUAL((char)q->bid_denom(), 'F');
		BOOST_CHECK_EQUAL((char)q->get_type(), 'D');
		BOOST_CHECK_EQUAL((char)q->get_categ(), 'L');
		processor.wr_quote( q );
		ofs.flush();

		BOOST_CHECK_THROW(
			q->read(),
			std::logic_error
		);
		ofs.close();

	}
	{
		string str = data_path + string("233.200.79.128.udp");
		ifstream fs; 
		fs.open (str.c_str());
		trade t(fs);
		stringstream sout;
		copy(istreambuf_iterator<char>(fs),
			istreambuf_iterator<char>(),
			ostreambuf_iterator<char>(sout));
		vector_messages msgs;
		message::divide_messages(msgs, boost::shared_ptr<string>(new string(sout.str())), false);

	}

	{
		BOOST_CHECK_THROW(
			message::denominator('J'),
			std::logic_error
		);
		BOOST_CHECK_EQUAL(message::denominator('8'), 256.0);
		BOOST_CHECK_EQUAL(message::denominator('C'), 1000.0);
		BOOST_CHECK_EQUAL(message::denominator('H'), 100000000.0);
		BOOST_CHECK_EQUAL(message::denominator('I'), 1.0);
	}
	{
		const quote::quote_t& quot = quote::get_long();
		BOOST_CHECK_EQUAL(quot.off_denom_of, 36);
		BOOST_CHECK_EQUAL(quot.bid_vol_len, quot.off_vol_len);
		BOOST_CHECK_EQUAL(quot.bid_vol_len, 7);

		string str = data_path + string("233.200.79.0.udp");
		ifstream inp; 
		inp.open (str.c_str());
		quote q(inp);
		q.read();
		while( q.get_categ() != message::end_reached)
		{
			q.categ = message::empty;
			q.read();
			char c = 0;
			while(c != start)
			{
				q.get_char(c);
				if(inp.peek() == EOF)
				{
					break;
				}
			}
			if(inp.peek() != EOF)
			{
				inp.seekg((size_t)inp.tellg() - 1);
			}
			else break;
		}
	}
	
	{
		const trade::trade_t* t1 = & trade::get_short() ;
		const trade::trade_t& t2 = trade::get_long();
		BOOST_CHECK_EQUAL(t1->denom_of, 5) ;
		BOOST_CHECK_EQUAL(t1->pr_of, 6) ;
		BOOST_CHECK_EQUAL(t1->vol_len, 4);
		BOOST_CHECK_EQUAL(t2.vol_of, 34) ;
		BOOST_CHECK_EQUAL(t2.pr_of, 22) ;
		BOOST_CHECK_EQUAL(t2.denom_of, 21) ;
		string str = data_path + string("233.200.79.0.udp");
		ifstream fs; 
		stringstream sout;
		fs.open (str.c_str());
		copy(istreambuf_iterator<char>(fs),
			istreambuf_iterator<char>(),
			ostreambuf_iterator<char>(sout));
		message m(sout);
		while(m.read_header() != message::end_reached)
		{
			m.categ = message::empty;
			char c = 0;
			while(c != start)
			{
				if(sout.peek() == EOF)
				{
					break;
				}
				m.get_char(c);
			}

			if(sout.peek() != EOF)
			{
				sout.seekg((size_t)sout.tellg() - 1);
			}
			else break;
		}
	}

	{
		string str = data_path + string("233.200.79.128.udp");
		ifstream fs; 
		fs.open (str.c_str());
		trade t(fs);
		t.read();
		while( t.get_categ() != message::end_reached)
		{
			t.categ = message::empty;
			t.read();
			char c = 0;
			while(c != start)
			{
				if(fs.peek() == EOF)
				{
					break;
				}
				t.get_char(c);
			}
			if(fs.peek() != EOF)
			{
				fs.seekg((size_t)fs.tellg() - 1);
			}
			else break;
		}
	}
}
