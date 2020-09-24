#include <tins/tins.h>

#include <iomanip>
#include <iostream>

#include "cec/cec.hpp"
#include "cec/module_filter.hpp"
#include "iface/physical.hpp"
#include "iface/streetpass.hpp"
#include "nl80211/socket.hpp"

using namespace streetpass;

int main(int argc, char** argv) {
  iface::PhysicalInterface phys(3);
  iface::StreetpassInterface siface = phys.setup_streetpass_interface();
  auto res = siface.scan(5000);

  for (auto [hwaddr, module_filter] : res) {
    std::cout << hwaddr << std::endl;
    std::cout << module_filter << std::endl;
  }
  /*std::vector<uint8_t> d = {0x11, 0x0D, 0x00, 0x05, 0x16, 0x00, 0x31,
                            0xFF, 0xEE, 0xDD, 0x00, 0x02, 0x08, 0x00,
                            0x00, 0xf0, 0x08, 0x68, 0xc7, 0x27, 0x39,
                            0x0e, 0x2f, 0xbb, 0x04};
  int i = 0;
  for (auto b : d) {
    if (i % 16 == 0) std::cout << std::endl;
    std::cout << std::hex << std::setfill('0') << std::setw(2) << (unsigned)b
              << " ";
    i = (i + 1) % 16;
  }
  try {
    cec::ModuleFilter test = cec::Parser<cec::ModuleFilter>::from_bytes(d);
    i = 0;
    for (auto b : test.to_bytes()) {
      if (i % 16 == 0) std::cout << std::endl;
      std::cout << std::hex << std::setfill('0') << std::setw(2) << (unsigned)b
                << " ";
      i = (i + 1) % 16;
    }
    std::cout << std::endl << test << std::endl;
  } catch (const char* c) {
    std::cerr << c << std::endl;
  }*/
  //}
  return 0;
}
