#include "iface/streetpass.hpp"
#include "nl80211/message.hpp"

#include <thread>
#include <chrono>
#include <tins/tins.h>

namespace streetpass::iface {

  const std::string StreetpassInterface::SSID = "Nintendo_3DS_continuous_scan_000";

  StreetpassInterface::StreetpassInterface(PhysicalInterface const& phys, std::string const& name) {
    nl80211::wiface w = nl80211::commands::new_interface(nlsock, phys.get_id(), NL80211_IFTYPE_ADHOC, name, true);
    m_index = w.index;
    std::this_thread::sleep_for (std::chrono::seconds(1));
    down();
    nl80211::commands::set_interface_mode(nlsock, m_index, NL80211_IFTYPE_ADHOC);
    std::this_thread::sleep_for (std::chrono::seconds(1));
    up();
    nl80211::commands::join_ibss(nlsock, m_index, SSID, 2412, true, w.mac);
  }

  void StreetpassInterface::scan() {
    nl80211::Socket scan_sock;
    std::cout << std::hex << (Tins::Dot11::Types::MANAGEMENT | Tins::Dot11::ManagementSubtypes::PROBE_REQ << 4) << std::endl;
    nl80211::commands::register_frame(scan_sock, m_index,
      Tins::Dot11::Types::MANAGEMENT | (Tins::Dot11::ManagementSubtypes::PROBE_REQ << 4));

    auto handler = [](nl80211::MessageParser& msg, void*) {
      std::vector<std::uint8_t> data;
      try {
        data = msg.get<std::vector<std::uint8_t>>(NL80211_ATTR_FRAME).value();
      } catch(...) {
        std::cerr << "Not a probe request frame" << std::endl;
        return;
      }

      Tins::Dot11ProbeRequest probereq(data.data(), data.size());
      std::cout << "-- Found Probe Request --" << std::endl;
      std::cout << probereq.ssid() << std::endl;
      throw "STOP";
    };

    scan_sock.recv_messages(handler, nullptr, true);
  }
}
