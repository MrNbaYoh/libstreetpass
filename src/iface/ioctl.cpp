#include "iface/ioctl.hpp"

#include <net/if.h>
#include <net/if_arp.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <cstring>
#include <system_error>

namespace streetpass::ioctl {

Socket::Socket() {
  sock = socket(PF_INET, SOCK_DGRAM, 0);
  if (sock == -1) throw std::system_error(errno, std::generic_category());
}

Socket::~Socket() { close(sock); }

int Socket::get_fd() const { return sock; }

namespace {
short get_interface_flags(Socket const& socket, std::string const& if_name) {
  struct ifreq ifr = {};

  if_name.copy(ifr.ifr_name, IFNAMSIZ - 1, 0);
  int ret = ::ioctl(socket.get_fd(), SIOCGIFFLAGS, &ifr);
  if (ret < 0) throw std::system_error(errno, std::generic_category());

  return ifr.ifr_flags;
}

void set_interface_flags(Socket const& socket, std::string const& if_name,
                         short flags) {
  struct ifreq ifr = {};

  ifr.ifr_flags = flags;
  if_name.copy(ifr.ifr_name, IFNAMSIZ - 1);
  int ret = ::ioctl(socket.get_fd(), SIOCSIFFLAGS, &ifr);
  if (ret < 0) throw std::system_error(errno, std::generic_category());
}
}  // namespace

bool is_interface_up(Socket const& socket, std::string const& if_name) {
  short flags = get_interface_flags(socket, if_name);
  return flags & IFF_UP;
}

void set_interface_up(Socket const& socket, std::string const& if_name) {
  short flags = get_interface_flags(socket, if_name);
  set_interface_flags(socket, if_name, flags | IFF_UP);
}

void set_interface_down(Socket const& socket, std::string const& if_name) {
  short flags = get_interface_flags(socket, if_name);
  set_interface_flags(socket, if_name, flags & ~IFF_UP);
}

std::array<std::uint8_t, 6> get_interface_hwaddr(Socket const& socket,
                                                 std::string const& if_name) {
  struct ifreq ifr = {};

  if_name.copy(ifr.ifr_name, IFNAMSIZ - 1);
  int ret = ::ioctl(socket.get_fd(), SIOCGIFHWADDR, &ifr);
  if (ret < 0) throw std::system_error(errno, std::generic_category());

  std::array<std::uint8_t, 6> addr;
  std::copy(ifr.ifr_hwaddr.sa_data, ifr.ifr_hwaddr.sa_data + 6,
            std::begin(addr));
  return addr;
}

void set_interface_hwaddr(Socket const& socket, std::string const& if_name,
                          std::array<std::uint8_t, 6> const& addr) {
  struct ifreq ifr = {};

  if_name.copy(ifr.ifr_name, IFNAMSIZ - 1);
  std::copy(std::begin(addr), std::end(addr), ifr.ifr_hwaddr.sa_data);
  ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;

  int ret = ::ioctl(socket.get_fd(), SIOCSIFHWADDR, &ifr);
  if (ret < 0) throw std::system_error(errno, std::generic_category());
}

int get_interface_index(Socket const& socket, std::string const& if_name) {
  struct ifreq ifr = {};

  if_name.copy(ifr.ifr_name, IFNAMSIZ - 1);
  int ret = ::ioctl(socket.get_fd(), SIOCGIFINDEX, &ifr);
  if (ret < 0) throw std::system_error(errno, std::generic_category());

  return ifr.ifr_ifindex;
}
}  // namespace streetpass::ioctl
