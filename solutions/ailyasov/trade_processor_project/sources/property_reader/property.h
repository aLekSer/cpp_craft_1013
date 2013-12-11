#ifndef TRADE_PROCESSOR_PROJECT_PROPERTY_
#define TRADE_PROCESSOR_PROJECT_PROPERTY_

#include <string>
#include <vector>
#include <utility>

namespace property
{ 
    class reader
    {
        typedef std::pair<std::string, unsigned short> addr_;
        public:
            reader(std::string const&);
            size_t trade_thread_size() const;
            size_t quote_thread_size() const;
            size_t trade_ports_amount() const;
            bool opened() const;
            size_t quote_ports_amount() const;
            std::vector<addr_> const& trades() const;
            std::vector<addr_> const& quotes() const;
        private:
            bool opened_;
            void parse(std::ifstream&, std::vector<addr_>&, size_t&);
            size_t trade_thread_size_;
            size_t quote_thread_size_;
            size_t trade_ports_amount_;
            size_t quote_ports_amount_; 
            std::vector<addr_> trades_;
            std::vector<addr_> quotes_; 
    };

}
#endif
