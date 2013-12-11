#ifndef _CONFIG_H_
#define _CONFIG_H_
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

typedef string ip_address;
typedef unsigned short port;
typedef vector<pair<ip_address, port>> addresses;
//static const string data_path ("D:/CppCraft/solutions/aleks_apalikov/Craft_6/sources/data/") ;
//static const string data_path ("F:/Cpp_Craft/cpp/solutions/aleks_apalikov/Craft_6/sources/data/") ;
static const string bin_path (BINARY_DIR "/");
static const string data_path (SOURCE_DIR "/sources/data/");

class config
{
	string file_name;
	ifstream conf;
	size_t trade_thread_size;
	size_t quote_thread_size;
	size_t trade_ports_amount;
	addresses trades;
	size_t quote_ports_amount;
	addresses quotes;

public:
	explicit config(string& name):file_name(name)
	{
		conf.open(file_name.c_str());
		if(!conf.is_open())
		{
			cout << "Config file "<< name.c_str() <<" was not open!" << endl;
			return;
		}
		conf >> trade_thread_size;
		conf >> quote_thread_size;
		conf >> trade_ports_amount;
		const size_t ip_char_len = 16;
		for(size_t i = 0; i < trade_ports_amount; i++)
		{
			port num;
			char address_line[ip_char_len];
			conf.getline(address_line, ip_char_len, ' ');
			ip_address ip(address_line + 1); // + 1 - to del \n
			conf >> num;
			trades.push_back(make_pair(ip, num));
		}
		conf >> quote_ports_amount;
		for(size_t i = 0; i < quote_ports_amount; i++)
		{
			port num;
			char address_line[ip_char_len];
			conf.getline(address_line, ip_char_len, ' ');
			ip_address ip(address_line + 1);
			conf >> num;
			quotes.push_back(make_pair(ip, num));
		}

	}
	const addresses& get_trades()
	{
		return trades;
	}
	const addresses& get_quotes()
	{
		return quotes;
	}
	size_t trade_threads()
	{
		return trade_thread_size;
	}
	size_t quote_threads()
	{
		return quote_thread_size;
	}
	size_t trade_ports()
	{
		return trade_ports_amount;
	}
	size_t quote_ports()
	{
		return quote_ports_amount;
	}
	~config( );
};

#endif //_CONFIG_H_