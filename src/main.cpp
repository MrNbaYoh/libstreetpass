#include <iostream>
#include <iomanip>
#include "utils/endianness.hpp"
#include "nl80211/socket.hpp"

int main(int argc, char** argv) {
  nl80211::Socket nlsock;
  std::cout << nlsock.get_driver_id() << std::endl;

  return 0;
}
