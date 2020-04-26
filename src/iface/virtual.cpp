#include "iface/virtual.hpp"
#include "iface/ioctl.hpp"

namespace streetpass::iface {
  VirtualInterface::VirtualInterface() : m_index(-1) {}

  VirtualInterface::VirtualInterface(std::uint32_t index) : m_index(index) {}

  nl80211::wiface VirtualInterface::get_all_info(std::uint32_t index) {
    nl80211::Socket nlsock;
    return nl80211::commands::get_interface(nlsock, index);
  }

  std::array<std::uint8_t, 6> VirtualInterface::get_mac_addr() const {
    return get_all_info(m_index).mac;
  }

  std::string VirtualInterface::get_name() const {
    return get_all_info(m_index).name;
  }

  bool VirtualInterface::is_up() const {
    //TODO: exception handling?
    ioctl::Socket sock;
    return ioctl::is_interface_up(sock, get_name());
  }

  void VirtualInterface::up() const {
    //TODO: exception handling?
    ioctl::Socket sock;
    ioctl::set_interface_up(sock, get_name());
  }

  void VirtualInterface::down() const {
    //TODO: exception handling?
    ioctl::Socket sock;
    ioctl::set_interface_down(sock, get_name());
  }
}
