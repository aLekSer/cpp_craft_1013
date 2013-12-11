#include "property.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <stdexcept>

namespace property
{
    void reader::parse(std::ifstream &in, std::vector<addr_>& v, size_t& count)
    {
        std::stringstream ss;
        std::string line;
        std::getline(in, line);
        ss.clear();
        ss.str(line); 
        ss >> count;
        for(size_t i = 0; i < count; ++i)
        {
            std::getline(in, line);
            ss.clear();
            ss.str(line);
            std::string addr;
            unsigned short port;
            ss >> addr;
            ss >> port;
            v.push_back(std::make_pair(addr, port));
        }
    }

    reader::reader(std::string const& filename) : 
            opened_(false),
            trade_thread_size_(0), quote_thread_size_(0), 
            trade_ports_amount_(0), quote_ports_amount_ (0)
    {

        std::ifstream in(filename, std::ios_base::in);
        std::string line;
        if(!in.is_open())
        {
            std::cerr << "error reading file " << filename << std::endl; 
        }
        opened_ = true;
        std::stringstream ss;
        std::getline(in, line);
        ss.str(line);
        ss >> trade_thread_size_;

        std::getline(in, line);
        ss.clear();
        ss.str(line); 
        ss >> quote_thread_size_; 

        parse(in, trades_, trade_ports_amount_);
        parse(in, quotes_, quote_ports_amount_);
    } 

    bool reader::opened() const
    {
        return opened_;
    } 

    size_t reader::trade_thread_size() const
    {
        return trade_thread_size_;
    }
    size_t reader::quote_thread_size() const
    {
        return quote_thread_size_;
    }

    size_t reader::trade_ports_amount() const
    {
        return trade_ports_amount_;
    }
    size_t reader::quote_ports_amount() const
    {
        return quote_ports_amount_;
    }
    std::vector<reader::addr_> const& reader::trades() const
    {
        return trades_;
    }
    std::vector<reader::addr_> const& reader::quotes() const
    {
        return quotes_;
    }

}
