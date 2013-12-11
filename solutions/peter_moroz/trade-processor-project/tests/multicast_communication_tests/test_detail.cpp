#include "test_detail.h"

void multicast_communication::tests_::detail::parse_datasource(std::ifstream& ifs,
					      std::vector<std::string>& datablocks)
{
  datablocks.clear();
  std::string block;
  while (!ifs.eof())
  {
    char x = '\0';
    ifs.read(&x, sizeof(x));
    if (ifs.good())
    {
      if (x == 0x01)
      {
        block.clear();
        block.push_back(x);
      }
      else
      {
        block.push_back(x);
        if (x == 0x03)
          datablocks.push_back(block);
      }
    }
  } // while (!ifs.eof())
}

