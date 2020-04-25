#include <iostream>
#include <iomanip>
#include "nl80211/socket.hpp"
#include "iface/interface.hpp"

using namespace streetpass;

int main(int argc, char** argv) {
  iface::Physical phys(12);
  phys.setup_streetpass_interface();
  return 0;
}
