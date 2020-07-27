#pragma once

#include <tins/tins.h>

#include <string>

#include "iface/physical.hpp"
#include "iface/virtual.hpp"
#include "nl80211/socket.hpp"

namespace streetpass::iface {
class StreetpassInterface : public VirtualInterface {
 private:
  nl80211::Socket nlsock;
  StreetpassInterface(PhysicalInterface const& phys, std::string const& name);
  friend class PhysicalInterface;

 public:
  StreetpassInterface(const StreetpassInterface&) = delete;
  StreetpassInterface& operator=(const StreetpassInterface&) = delete;
  StreetpassInterface(StreetpassInterface&&) = delete;
  StreetpassInterface& operator=(StreetpassInterface&&) = delete;

  std::map<Tins::HWAddress<6>, std::vector<std::uint8_t>> scan(
      unsigned int ms_duration);

  static const std::string SSID;
  static const Tins::HWAddress<3> OUI;
};
}  // namespace streetpass::iface
