#include "iface/streetpass.hpp"

#include <tins/tins.h>

#include <chrono>
#include <thread>

#include "nl80211/message.hpp"

namespace streetpass::iface {

const std::string StreetpassInterface::SSID("Nintendo_3DS_continuous_scan_000");
const Tins::HWAddress<3> StreetpassInterface::OUI("00:1f:32");
const Tins::Dot11ManagementFrame::rates_type
    StreetpassInterface::SUPPORTED_RATES({1.0, 2.0, 5.5, 6.0, 9.0, 11.0, 12.0,
                                          18.0});
const Tins::Dot11ManagementFrame::rates_type
    StreetpassInterface::EXT_SUPPORTED_RATES({24.0, 36.0, 48.0, 54.0});
const int StreetpassInterface::CHANNEL_FREQ = 2412;

StreetpassInterface::StreetpassInterface(PhysicalInterface const& phys,
                                         std::string const& name) {
  // TODO: handle exception
  nl80211::wiface w = nl80211::commands::new_interface(
      nlsock, phys.get_id(), NL80211_IFTYPE_ADHOC, name, true);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  m_index = w.index;

  w = nl80211::commands::get_interface(nlsock, m_index);
  if (w.type != NL80211_IFTYPE_ADHOC) {
    down();
    nl80211::commands::set_interface_mode(nlsock, m_index,
                                          NL80211_IFTYPE_ADHOC);
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  if (!is_up()) up();

  nl80211::commands::join_ibss(nlsock, m_index, SSID, CHANNEL_FREQ, true,
                               w.mac);
}

namespace {
bool is_streetpass_scan_probereq(Tins::Dot11ProbeRequest const& probereq) {
  try {
    return probereq.vendor_specific().oui == StreetpassInterface::OUI &&
           probereq.addr1().is_broadcast() &&
           probereq.ssid() == StreetpassInterface::SSID;
  } catch (Tins::option_not_found&) {
    return false;
  }
}
}  // namespace

void StreetpassInterface::scan(
    unsigned int ms_duration,
    std::function<bool(Tins::HWAddress<6> const&,
                       cec::ModuleFilter const&)> const& filter,
    std::function<void(Tins::HWAddress<6> const&,
                       cec::ModuleFilter const&)> const& callback) {
  if (ms_duration == 0) return;

  // TODO: handle exception
  nl80211::Socket scan_sock;
  nl80211::commands::register_frame(
      scan_sock, m_index,
      Tins::Dot11::Types::MANAGEMENT |
          (Tins::Dot11::ManagementSubtypes::PROBE_REQ << 4));

  auto handler = [filter, callback](nl80211::MessageParser& msg, void*) {
    std::vector<std::uint8_t> data;
    try {
      data = msg.get<std::vector<std::uint8_t>>(NL80211_ATTR_FRAME).value();
    } catch (...) {
      return;
    }

    Tins::Dot11ProbeRequest probereq(data.data(), data.size());
    if (!is_streetpass_scan_probereq(probereq)) return;

    std::cout << "-- Found Streetpass Probe Request --" << std::endl;
    auto vendor_specific_data = probereq.vendor_specific().data;

    // TODO: first byte of vendor specific data is always 0x01?
    if (vendor_specific_data.size() == 0 || vendor_specific_data.at(0) != 0x01)
      return;

    auto module_filter_bytes = &vendor_specific_data[1];
    unsigned module_filter_bytes_size = vendor_specific_data.size() - 1;
    try {
      auto peer_addr = probereq.addr2();
      cec::ModuleFilter module_filter =
          cec::Parser<cec::ModuleFilter>::from_bytes(module_filter_bytes,
                                                     module_filter_bytes_size);
      if (filter(peer_addr, module_filter)) callback(peer_addr, module_filter);
    } catch (...) {
      return;
    }
  };

  scan_sock.recv_messages(handler, nullptr, true, ms_duration);
}

std::map<Tins::HWAddress<6>, cec::ModuleFilter> StreetpassInterface::scan(
    unsigned int ms_duration,
    std::function<bool(Tins::HWAddress<6> const&,
                       cec::ModuleFilter const&)> const& filter) {
  std::map<Tins::HWAddress<6>, cec::ModuleFilter> results;
  auto callback = [&results](Tins::HWAddress<6> const& peer_addr,
                             cec::ModuleFilter const& module_filter) {
    results.emplace(peer_addr, module_filter);
  };

  scan(ms_duration, filter, callback);
  return results;
}

std::map<Tins::HWAddress<6>, cec::ModuleFilter> StreetpassInterface::scan(
    unsigned int ms_duration) {
  auto no_filter = [](Tins::HWAddress<6> const&, cec::ModuleFilter const&) {
    return true;
  };

  return scan(ms_duration, no_filter);
}

std::map<Tins::HWAddress<6>, cec::ModuleFilter> StreetpassInterface::scan(
    unsigned int ms_duration, cec::ModuleFilter const& module_filter) {
  auto filter_match = [module_filter](Tins::HWAddress<6> const&,
                                      cec::ModuleFilter const& other) {
    return module_filter.match(other);
  };

  return scan(ms_duration, filter_match);
}

}  // namespace streetpass::iface
