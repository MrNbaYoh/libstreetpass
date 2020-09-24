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

  std::map<Tins::HWAddress<6>, cec::ModuleFilter> scan(
      unsigned int ms_duration,
      std::function<bool(Tins::HWAddress<6> const&,
                         cec::ModuleFilter const&)> const& filter);
  std::map<Tins::HWAddress<6>, cec::ModuleFilter> scan(
      unsigned int ms_duration);
  std::map<Tins::HWAddress<6>, cec::ModuleFilter> scan(
      unsigned int ms_duration, cec::ModuleFilter const& module_filter);

  static const std::string SSID;
  static const Tins::HWAddress<3> OUI;
  static const Tins::Dot11ManagementFrame::rates_type SUPPORTED_RATES;
  static const Tins::Dot11ManagementFrame::rates_type EXT_SUPPORTED_RATES;
  static const int CHANNEL_FREQ;
};
}  // namespace streetpass::iface
