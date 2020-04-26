#pragma once

#include <string>

#include "nl80211/socket.hpp"
#include "iface/physical.hpp"
#include "iface/virtual.hpp"
#include <tins/tins.h>

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

    void scan();

    static const std::string SSID;
    static const Tins::HWAddress<3> OUI;
  };
}
