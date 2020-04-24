#include <iostream>
#include <iomanip>
#include "utils/endianness.hpp"
#include "nl80211/socket.hpp"
#include "iface/interface.hpp"

using namespace streetpass;

int main(int argc, char** argv) {
  auto v = iface::Physical::find_all_supported();
  for(auto e: v)
    for(auto i: e.find_all_virtual())
      std::cout << i.get_name() << std::endl;

  return 0;
}
