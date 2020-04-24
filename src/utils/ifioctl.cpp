#include "utils/ifioctl.hpp"

#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <cstring>
#include <system_error>

namespace streetpass::ifioctl {
  namespace {
    short get_interface_flags(int socket, std::string const& if_name) {
      struct ifreq ifr = {};

    	if_name.copy(ifr.ifr_name, IFNAMSIZ - 1, 0);
    	int ret = ioctl(socket, SIOCGIFFLAGS, &ifr);
    	if(ret < 0)
    		throw std::system_error(errno, std::generic_category());

    	return ifr.ifr_flags;
    }

    void set_interface_flags(int socket, std::string const& if_name, short flags) {
      struct ifreq ifr = {};

    	ifr.ifr_flags = flags;
    	if_name.copy(ifr.ifr_name, IFNAMSIZ - 1);
    	int ret = ioctl(socket, SIOCSIFFLAGS, &ifr);
    	if(ret < 0)
    		throw std::system_error(errno, std::generic_category());
    }
  }

  int socket() {
    int res = socket(PF_INET, SOCK_DGRAM, 0);
    if(res == -1)
      throw std::system_error(errno, std::generic_category());
    return res;
  }

  void set_interface_up(int socket, std::string const& if_name) {
  	short flags = get_interface_flags(socket, if_name);
  	set_interface_flags(socket, if_name, flags | IFF_UP);
  }

  void set_interface_down(int socket, std::string const& if_name) {
    short flags = get_interface_flags(socket, if_name);
  	set_interface_flags(socket, if_name, flags & ~IFF_UP);
  }

  std::array<std::uint8_t, 6> get_interface_hwaddr(int socket, std::string const& if_name) {
    struct ifreq ifr = {};

  	if_name.copy(ifr.ifr_name, IFNAMSIZ - 1);
  	int ret = ioctl(socket, SIOCGIFHWADDR, &ifr);
  	if(ret < 0)
  		throw std::system_error(errno, std::generic_category());

    std::array<std::uint8_t, 6> addr;
    std::copy(ifr.ifr_hwaddr.sa_data, ifr.ifr_hwaddr.sa_data + 6, std::begin(addr));
  	return addr;
  }

  void set_interface_hwaddr(int socket, std::string const& if_name, std::array<std::uint8_t, 6> const& addr) {
    struct ifreq ifr = {};

  	if_name.copy(ifr.ifr_name, IFNAMSIZ - 1);
  	std::copy(std::begin(addr), std::end(addr), ifr.ifr_hwaddr.sa_data);
  	ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;

  	int ret = ioctl(socket, SIOCSIFHWADDR, &ifr);
  	if(ret < 0)
  		throw std::system_error(errno, std::generic_category());
  }

  int get_interface_index(int socket, std::string const& if_name) {
    struct ifreq ifr = {};

    if_name.copy(ifr.ifr_name, IFNAMSIZ - 1);
    int ret = ioctl(socket, SIOCGIFINDEX, &ifr);
    if(ret < 0)
      throw std::system_error(errno, std::generic_category());

    return ifr.ifr_ifindex;
  }
}
