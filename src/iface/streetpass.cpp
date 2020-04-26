#include "iface/streetpass.hpp"

#include <thread>
#include <chrono>

namespace streetpass::iface {

  StreetpassInterface::StreetpassInterface(PhysicalInterface const& phys, std::string const& name) {
    nl80211::wiface w = nl80211::commands::new_interface(nlsock, phys.get_id(), NL80211_IFTYPE_ADHOC, name, true);
    m_index = w.index;
    std::this_thread::sleep_for (std::chrono::seconds(1));
    down();
    nl80211::commands::set_interface_mode(nlsock, m_index, NL80211_IFTYPE_ADHOC);
    std::this_thread::sleep_for (std::chrono::seconds(1));
    up();
    nl80211::commands::join_ibss(nlsock, m_index, "Nintendo_3DS_continuous_scan_000", 2412, true, w.mac);
  }
}
