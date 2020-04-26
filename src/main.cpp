#include <iostream>
#include <iomanip>
#include "nl80211/socket.hpp"
#include "iface/physical.hpp"
#include "iface/streetpass.hpp"
#include <tins/tins.h>

using namespace streetpass;

int main(int argc, char** argv) {
  iface::PhysicalInterface phys(12);
  iface::StreetpassInterface siface = phys.setup_streetpass_interface();
  siface.scan();
  return 0;
}
