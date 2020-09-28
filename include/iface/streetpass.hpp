#pragma once

#include <tins/tins.h>

#include <string>

#include "cec/module_filter.hpp"
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

  void scan(unsigned int timeout,
            std::function<bool(Tins::HWAddress<6> const&,
                               cec::ModuleFilter const&)> const& filter,
            std::function<void(Tins::HWAddress<6> const&,
                               cec::ModuleFilter const&)> const& callback);
  std::map<Tins::HWAddress<6>, cec::ModuleFilter> scan(
      unsigned int timeout,
      std::function<bool(Tins::HWAddress<6> const&,
                         cec::ModuleFilter const&)> const& filter);
  std::map<Tins::HWAddress<6>, cec::ModuleFilter> scan(unsigned int timeout);
  std::map<Tins::HWAddress<6>, cec::ModuleFilter> scan(
      unsigned int timeout, cec::ModuleFilter const& module_filter);

  static const std::string SSID;
  static const Tins::HWAddress<3> OUI;
  static const Tins::Dot11ManagementFrame::rates_type SUPPORTED_RATES;
  static const Tins::Dot11ManagementFrame::rates_type EXT_SUPPORTED_RATES;
  static const int CHANNEL_FREQ;
};
}  // namespace streetpass::iface
