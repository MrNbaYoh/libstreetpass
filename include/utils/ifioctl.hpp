#pragma once

#include <array>
#include <string>
#include <cstdint>

namespace streetpass::ifioctl {
  int socket();
  void set_interface_up(int socket, std::string const& if_name);
  void set_interface_down(int socket, std::string const& if_name);
  std::array<std::uint8_t, 6> get_interface_hwaddr(int socket, std::string const& if_name);
  void set_interface_hwaddr(int socket, std::string const& if_name, std::array<std::uint8_t, 6> const& addr);
  int get_interface_index(int socket, std::string const& if_name);
}
