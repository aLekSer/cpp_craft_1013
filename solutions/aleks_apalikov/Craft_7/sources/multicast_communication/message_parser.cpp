#include "message_parser.h"
#include <stdexcept>

int message::counter = 0;
message::message_category message::read_header()
{
	streamoff cur_pos = inp.tellg();
	if(inp.peek() == EOF)
		return end_reached;
	byte ch = 0;
	try
	{
		if(!inp)
			return end_reached;
		if( !inp_good )
			return end_reached; // from enum message_category
	}
	catch (exception e)
	{
		cout << e.what() << endl;
		return end_reached;		
	}
	get_byte(ch);
	bool test = (ch == start) || (ch == unit_separator);
	counter ++ ; // total delim:start 's
	place = 0;
	get_byte(ch);
	if((ch == bond)||(ch == equity)||(ch == local_issue))
	{
		categ = (message_category)ch;
	}
	else 
		return end_reached;

	byte b;
	get_byte(b); 
	typ = (message_type) b;
	inp.seekg(cur_pos + time_stamp + 1); //1 - start byte 0x1
	read_binary(inp, hour_minute);
	get_byte( sec_ );
	inp.seekg(cur_pos + header_len + 1);

	return categ;

}

message* message::read()
{
	if((categ == bond)||(categ == equity)||(categ == local_issue)||(categ == end_reached))
	{
		throw std::logic_error("Read only one time");
	}
	try{
		if(read_header() == -1) 
		{
			categ = end_reached;
			return NULL;
		}
		parse_rest();
		return this;
	}
	catch( std::exception e )
	{
		cout << e.what() << endl;
		categ = end_reached;
		return this;
	}
}

void message::get_byte(byte & ch)
{
	if((inp.eof()) || (inp.peek() == EOF))
	{
		inp.setstate(iostream::eofbit);
		ch = EOF;
		return;
	}
	read_binary(inp, ch);
	place++;
}

void message::get_char( char & ch )
{
	if((inp.eof()) || (inp.peek() == EOF))
	{
		inp.setstate(iostream::eofbit);
		ch = EOF;
		return;
	}
	read_binary(inp, ch);
	place++;
}
void message::read_block( stringstream& ss, ifstream& fs )
{
	char c;
	if(! fs.is_open())
	{
		cout << "udp file was not open";	
		return;
	}
	if(fs.peek() != EOF)
	{
		read_binary(fs, c);
		write_binary(ss, c);
	}
	while (c != end_of_text && fs.peek() != EOF) /*delim::*/
	{
		read_binary(fs, c);
		write_binary(ss, c);
	}
	if (ss.str().size() > 0)
		count++;
}


void message::get_string( string & s, size_t pos, size_t len )
{
	streamoff cur_pos = inp.tellg();
	inp.seekg(cur_pos + pos);
	char * cs = new char [len + 1];
	for (size_t i = 0; i < len; i++)
	{
		get_char(cs[i]);
	}
	cs[len] = '\0';
	s = string(cs);
	len = s.length();
	delete [] cs;
}

int message::parse_rest()
{
	return 0;

}

void message::divide_messages( vector_messages& vec_msgs, boost::shared_ptr<std::string> buffer, const bool quotes )
{
	stringstream current_message;
	for(string::iterator it = buffer->begin(); it != buffer->end(); it++)
	{
		if((*it == start) || (*it == unit_separator) ) //from enum delim
		{
			if(*it == unit_separator)
			{
				it++;
				current_message << start;
			}
			while( it != buffer->end())
			{
				current_message << *it;
				boost::shared_ptr<message> sm;
				if((*it == unit_separator) || (*it == end_of_text))
				{
					if(quotes)
					{
						boost::shared_ptr<quote> st (new quote(current_message));
						st->read( );
						if(st->get_categ() != end_reached)						
							sm = boost::static_pointer_cast<message, quote>(st);
						else 
							break;
					}
					else
					{
						boost::shared_ptr<trade> st (new trade(current_message));
						st->read( );
						
						if(st->get_categ() != end_reached)		
							sm = boost::static_pointer_cast<message, trade>(st);
						else
							break;
					}
					vec_msgs.push_back(sm);
					sm.reset();
					current_message.str(string()); //empty current_message
					break;
				}
				it++;
			}
			if(it != buffer->end() )
			{
				if((*it == unit_separator) && (it != buffer->begin()))
				{
					it--;
				}
				continue;
			}
			else break;
		}
	}
}

int message::count = 0;


int quote::parse_rest()
{
	if(inp.eof())
	{
		return -1;
	}
	map<char, int>::const_iterator i = sec_len.find(typ);
	if((i != sec_len.end()) && typ != 'I')
		get_string(security_symbol_, 0, i->second);
	else return -1;
	string st;
	const quote::quote_t* cur_trade;
	switch (i->first)
	{
	case short_quote: //enum message_type::short_quote
		cur_trade = &get_short();
		break;
	case long_quote:
		cur_trade = &get_long();
		break;
	default:
		cout<<"Unknown type read"<<endl;
		return -1;
	};
	parse(cur_trade);

	return 0;
}

void quote::parse( const quote::quote_t* cur_trade )
{
	streamoff sec_symb = inp.tellg();
	inp.seekg(cur_trade->bid_denom_of + sec_symb, ios_base::beg);
	get_char(bid_denom_);
	denominator(bid_denom_); //test if wrong than wouldn't output to file
	inp.seekg(cur_trade->bid_pr_of + sec_symb, ios_base::beg);
	string s;
	get_string(s, 0, cur_trade->bid_pr_len);
	bid_price_ = boost::lexical_cast<double> (s);
	inp.seekg(cur_trade->bid_vol_of + sec_symb, ios_base::beg);
	s.clear();
	get_string(s, 0, cur_trade->bid_vol_len);
	bid_volume_ = boost::lexical_cast<double> (s);
	inp.seekg(cur_trade->off_denom_of + sec_symb, ios_base::beg);
	get_char(offer_denom_);
	denominator(offer_denom_); //test
	inp.seekg(cur_trade->off_pr_of + sec_symb, ios_base::beg);
	s.clear();
	get_string(s, 0, cur_trade->off_pr_len);
	offer_price_ = boost::lexical_cast<double> (s);
	inp.seekg(cur_trade->off_vol_of + sec_symb, ios_base::beg);
	s.clear();
	get_string(s, 0, cur_trade->off_vol_len);
	offer_volume_ = boost::lexical_cast<double> (s);
}

// (char bvo, char bvl, char bpo, char bpl, char bdo, char ovo, char ovl, char opo, char opl, char odo) //starting from end of security symbol
const vector<quote::quote_t> quote::short_long = boost::assign::list_of (quote_t (12, 3, 4, 8, 3, 25, 3, 17, 8, 16 ) )
(quote_t ( 29, 7, 17, 12, 16, 49, 7, 37, 12, 36 ) );

const quote::quote_t& quote::get_short()
{
	return short_long[0];
}
const quote::quote_t& quote::get_long()
{
	return short_long[1];
}

int trade::parse_rest()
{
	if(inp.eof())
	{
		return -1;
	}
	map<char, int>::const_iterator i = sec_len.find(typ);
	if((i != sec_len.end()) && typ != 'D')
		get_string(security_symbol_, 0, i->second);
	else return -1;
	string st;
	const trade::trade_t* cur_trade;
	switch (i->first)
	{
	case short_trade: //enum message_type::short_trade
		cur_trade = &get_short();
		break;
	case long_trade:
		cur_trade = &get_long();
		break;
	default:
		cout<<"Unknown type read"<<endl;
		return -1;
	};
	parse(cur_trade);

	return 0;
}



vector<trade::trade_t> trade::short_long = boost::assign::list_of(trade_t(1, 4, 6, 8, 5)) (trade_t(34, 9, 22, 12, 21));

const trade::trade_t& trade::get_short()
{
	return short_long[0];
}
const trade::trade_t& trade::get_long()
{
	return short_long[1];
}

void trade::parse(const trade::trade_t* cur_trade )
{
	streamoff sec_symb = inp.tellg();
	inp.seekg(cur_trade->denom_of + sec_symb, ios_base::beg);
	get_char(denom_);
	denominator(denom_); //validate full message like in quote
	inp.seekg(cur_trade->pr_of + sec_symb, ios_base::beg);
	string s;
	get_string(s, 0, cur_trade->pr_len);
	price_ = boost::lexical_cast<double> (s);
	inp.seekg(cur_trade->vol_of + sec_symb, ios_base::beg);
	s.clear();
	get_string(s, 0, cur_trade->vol_len);
	volume_ = boost::lexical_cast<double> (s);
}
