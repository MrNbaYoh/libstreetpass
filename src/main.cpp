#include <iostream>
#include <iomanip>
#include "utils/endianness.hpp"
#include "nl80211/socket.hpp"
#include "iface/interface.hpp"

using namespace streetpass;

int main(int argc, char** argv) {
  iface::Physical phys(12);
  auto v = phys.find_all_virtual();
  for(auto e: v)
    std::cout << e.get_name() << std::endl;
  return 0;
}
