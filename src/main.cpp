#include <iostream>
#include <iomanip>
#include "utils/endianness.hpp"
#include "nl80211/socket.hpp"
#include "iface/interface.hpp"

using namespace streetpass;

int main(int argc, char** argv) {
  auto v = iface::Physical::find_all_supported();
  for(auto e: v)
    if(e.get_id() == 11)
      e.setup_streetpass_interface();

  return 0;
}
