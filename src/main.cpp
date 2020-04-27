#include <iostream>
#include <iomanip>
#include "nl80211/socket.hpp"
#include "iface/physical.hpp"
#include "iface/streetpass.hpp"
#include <tins/tins.h>

using namespace streetpass;

int main(int argc, char** argv) {
  iface::PhysicalInterface phys(13);
  iface::StreetpassInterface siface = phys.setup_streetpass_interface();
  auto res = siface.scan(5000);
  for(auto [hwaddr, data]: res)
  {
    std::cout << hwaddr;
    int i = 0;
    for(auto b: data) {
      if(i%16 == 0)
        std::cout << std::endl;
      std::cout << std::hex << std::setfill('0') << std::setw(2) << (unsigned)b << " ";
      i = (i+1)%16;
    }
    std::cout << std::endl;
  }
  return 0;
}
