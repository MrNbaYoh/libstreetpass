#include <iostream>
#include <iomanip>
#include "nl80211/socket.hpp"
#include "iface/physical.hpp"
#include "iface/streetpass.hpp"
#include <tins/tins.h>
#include "cec/module_filter.hpp"

using namespace streetpass;

int main(int argc, char** argv) {
  /*iface::PhysicalInterface phys(13);
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

    std::vector<uint8_t> d(data.begin()+1, data.end());*/
    std::vector<uint8_t> d = {0x11, 0x0d , 0x00 , 0x05 , 0x16 , 0x00 , 0x11 , 0x0F, 0x0D, 0x0E, 0x00 , 0x02 , 0x08 , 0x00 , 0x00 , 0xf0 , 0x08 , 0x68 , 0xc7 , 0x27 , 0x39 , 0x0e , 0x2f , 0xbb , 0x04};
    int i = 0;
    for(auto b: d) {
      if(i%16 == 0)
        std::cout << std::endl;
      std::cout << std::hex << std::setfill('0') << std::setw(2) << (unsigned)b << " ";
      i = (i+1)%16;
    }
    try {
      cec::ModuleFilter test(d);
      i = 0;
      for(auto b: test.to_bytes()) {
        if(i%16 == 0)
          std::cout << std::endl;
        std::cout << std::hex << std::setfill('0') << std::setw(2) << (unsigned)b << " ";
        i = (i+1)%16;
      }
      std::cout << std::endl << test << std::endl;
    } catch (const char* c) {
      std::cerr << c << std::endl;
    }
  //}
  return 0;
}
