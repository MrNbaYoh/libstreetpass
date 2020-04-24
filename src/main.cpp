#include <iostream>
#include <iomanip>
#include "utils/endianness.hpp"
#include "nl80211/socket.hpp"
#include "iface/interface.hpp"

using namespace streetpass;

int main(int argc, char** argv) {
  nl80211::Socket nlsock;
  nl80211::commands::get_wiphy(nlsock, 42);

  return 0;
}
