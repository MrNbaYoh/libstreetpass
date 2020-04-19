#pragma once

#include <array>
#include <string>
#include <cstdint>

namespace ifioctl {
  int socket();
  short get_interface_flags(int socket, std::string if_name);
  void set_interface_flags(int socket, std::string if_name, short flags);
  void set_interface_up(int socket, std::string if_name);
  void set_interface_down(int socket, std::string if_name);
  std::array<std::uint8_t, 6> get_interface_hwaddr(int socket, std::string if_name);
  void set_interface_hwaddr(int socket, std::string if_name, std::array<std::uint8_t, 6> const& addr);
}
