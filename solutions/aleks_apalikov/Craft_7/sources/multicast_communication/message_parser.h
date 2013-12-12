#ifndef _MESSAGE_PARSER_H_
#define _MESSAGE_PARSER_H_
#include <fstream>
#include <iostream>
#include <map>
#include "boost/assign.hpp"
#include "boost/lexical_cast.hpp"
#include <stdexcept>
#include <string>
#include "boost\shared_ptr.hpp"
#include "../../tests/multicast_communication_tests/test_registrator.h"
#include <stdint.h>
#include <time.h>

typedef unsigned char byte;
using namespace std;
enum delim
{
	start = 0x01,
	unit_separator = 0x1F,
	end_of_text = 0x03
};
class message;
typedef vector<boost::shared_ptr<message>> vector_messages;
template< class T >
void read_binary( std::istream& in, T& t, const size_t len = sizeof( T ) )
{
	in.read( reinterpret_cast< char* >( &t ), len );
}

struct times {
	uint16_t hour_minute;
	byte sec_;
	uint16_t ms;
	uint32_t relative() // to 1.1.1990
	{
		time_t tt;
		uint32_t ret_val;
		time(&tt);
		struct tm * ptm;
		ptm = gmtime(&tt);
		ret_val = ((ptm->tm_year) * 372 + ptm->tm_mon * 31 + ptm->tm_mday);
		ret_val = ret_val * 24 * 60 + (hour_minute >> 8) * 60 + (hour_minute & 0xFF);
		char debug_time[50]; //TODO del before pull request
		strftime(debug_time, 50, "%d-%m-%Y", ptm);
		return ret_val;
	}
};
template< class T >
void write_binary( std::ostream& out, T& t, const size_t len = sizeof( T ) )
{
	out.write( reinterpret_cast< const char* >( &t ), len );
}
// the stream that is passed in constructor should exist before read()
// operation, it is controlled by divide_messages()
class message
{
	friend void text_test::quote_trade_parse();
protected:
	istream& inp; //possible to change to shared_ptr<istream>
	size_t place; //where are we reading
	bool inp_good;
	string security_symbol_;
	enum message_category
	{
		bond = 'B',
		equity = 'E',
		local_issue = 'L',
		end_reached = -1,
		error_occured = -2,
		empty = 'Y'
	};
	message_category categ;
	enum message_type
	{
		long_quote = 'B',
		short_quote = 'D',
		long_trade = 'B',
		short_trade = 'I'
	};
	enum {
		time_stamp = 18,
		header_len = 24, //security_symbol start
		code_ts = 0x30
	};

	message_type typ;
	times t;
public:
	static void divide_messages(  vector_messages& vec_msgs, boost::shared_ptr<std::string> buffer,
		const bool quotes);
	const string & security_symbol()
	{
		return security_symbol_;
	}
	message_type get_type()
	{
		return typ;
	}
	uint16_t get_time()
	{
		return t.hour_minute;
	}

	uint32_t minute_of_day()
	{
		return (get_time() >> 8) * 60 + (get_time() & 0xFF);
	}
	uint32_t minute()
	{
		return t.relative();
	}
private:
	void read_time()
	{
		char ch1, ch2;
		get_char(ch1) ;
		get_char(ch2) ;
		t.hour_minute = ((ch1 - code_ts) << 8) + (ch2 - code_ts); // code_ts == ')'
		get_byte(t.sec_);
		t.sec_ -= code_ts;

		read_ms();
	}
	void read_ms()
	{
		char ch;
		get_char(ch) ;
		t.ms = ch - code_ts;
		for(int i = 0; i != 2; i++)
		{
			get_char(ch) ;
			t.ms = t.ms * 10 + ch - code_ts;
		}
	}
public:

	uint32_t msec()
	{
		return 1000 * t.sec_ + t.ms;
	}
	static double denominator(char code)
	{
		static const int denoms[19] = {1, 1, 1, 8, 16, 32, 64, 128, 256, 1, 1,
			10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000};
		int decimal = 10;
		if (code == '0') return -1.0;
		if( code == 'I' ) return 1.0;
		if( code >= '3' && code <= '9' ) 
		{
			code -= '1' - 1;
		}
		else if ( code >= 'A' && code <= 'H' )
		{
			code -= 'A' - 1 - decimal; 
		}
		else 
		{
			throw std::logic_error("Wrong denominator");
			return 1.0;
		}
		return (double) denoms[code];
	}
	static int counter;
	message(): inp(cin) // not necessary construct
	{
		categ = empty;
	}
	message(istream& inf): inp(inf)
	{
		if (inf.eof())
		{
			cout<< "Error in stringstream: it is empty "<<endl;
			inp_good = false;
			categ = end_reached; //from message_category
			return;
		}
		inp_good = true;
		place = 0;
		inp.seekg(place);
		categ = empty;

	}
	message_category read_header(); //search for listed above message_category
private:
	message* read(); //invoke only one time, after throw exception error already read
					//it is used in divide and tests
public:
	void get_byte(byte & b);
	void get_char(char & c);
	static void read_block(stringstream& ss, ifstream& fs);
	static int count;
	message_category get_categ()
	{
		return categ;
	}
	void get_string(string & s, size_t pos, size_t len);
	virtual int parse_rest();
};
class quote: public message
{
	char bid_denom_;
	double bid_price_;
	double bid_volume_;
	char offer_denom_;
	double offer_price_;
	double offer_volume_;
public:
	double bid_denom()
	{ return bid_denom_; }
	double bid_price()
	{ return bid_price_; }
	double bid_volume()
	{ return bid_volume_; }
	char offer_denom()
	{ return offer_denom_; }
	double offer_price()
	{ return offer_price_; }
	double offer_volume() 
	{ return offer_volume_; }
	struct quote_t //offset of bytes after sec_symb
	{
		char bid_vol_of;
		char bid_vol_len;
		char bid_pr_of;
		char bid_pr_len;
		char bid_denom_of;
		char off_vol_of;
		char off_vol_len;
		char off_pr_of;
		char off_pr_len;
		char off_denom_of;
		quote_t(char bvo, char bvl, char bpo, char bpl, char bdo, char ovo, char ovl, char opo, char opl, char odo): 
			bid_vol_of(bvo), bid_vol_len(bvl), bid_pr_of(bpo), bid_pr_len(bpl), bid_denom_of(bdo),
			off_vol_of(ovo), off_vol_len(ovl), off_pr_of(opo), off_pr_len(opl), off_denom_of(odo)
		{
		}
	} ;
private:
	virtual int parse_rest();
	void parse( const quote::quote_t* cur_trade );
	static const vector<quote_t> short_long;
public:
	const static quote_t& get_short();
	const static quote_t& get_long();
public:
	quote(istream& ifs): message(ifs)
	{}
};

class trade:public message
{
public:
	struct trade_t //offset of bytes after sec_symb
	{
		char vol_of;
		char vol_len;
		char pr_of;
		char pr_len;
		char denom_of;
		trade_t(char vo, char vl, char po, char pl, char d_o): vol_of(vo), vol_len(vl), pr_of(po), pr_len(pl), denom_of(d_o)
		{
		}
	} ;
private:
	char denom_;
	double price_;
	double volume_;
public:
	char  denom()
	{ return denom_; }
	double price()
	{ return  price_; }
	double volume()
	{ return volume_; }
	static vector<trade_t> short_long;
	const static trade_t& get_short();
	const static trade_t& get_long();
	trade(istream& ifs): message(ifs)
	{
	}
	virtual int parse_rest();
	void parse(const trade::trade_t* cur_trade );
};
typedef boost::shared_ptr<quote> shared_quote;
typedef boost::shared_ptr<trade> shared_trade;
static const map<char, int> sec_len = boost::assign::map_list_of('D', 3) ('B', 11) ('I', 3);
#endif
