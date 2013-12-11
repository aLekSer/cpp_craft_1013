#ifndef MULTICAST_COMMUNICATION_TESTS_DETAIL_H_
#define MULTICAST_COMMUNICATION_TESTS_DETAIL_H_

#include <fstream>
#include <string>
#include <vector>

namespace multicast_communication
{
  namespace tests_
  {
    namespace detail
    {
      // auxiliary function, which provides functionality of parsing datasource file 
      void parse_datasource(std::ifstream& ifs, std::vector<std::string>& datablocks);
    }
  }
}


#endif // MULTICAST_COMMUNICATION_TESTS_DETAIL_H_
