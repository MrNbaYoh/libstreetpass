#include "iface/interface.hpp"
#include "utils/ifioctl.hpp"

#include <algorithm>
#include <iostream>
#include <thread>
#include <chrono>

namespace streetpass::iface {

  Virtual::Virtual(std::uint32_t index) : m_index(index) {}

  nl80211::wiface Virtual::get_all_info(std::uint32_t index) {
    nl80211::Socket nlsock;
    return nl80211::commands::get_interface(nlsock, index);
  }

  std::array<std::uint8_t, 6> Virtual::get_mac_addr() const {
    return get_all_info(m_index).mac;
  }

  std::string Virtual::get_name() const {
    return get_all_info(m_index).name;
  }

  void Virtual::up() const {
    //TODO: exception handling?
    ifioctl::Socket sock;
    ifioctl::set_interface_up(sock, get_name());
  }

  void Virtual::down() const {
    //TODO: exception handling?
    ifioctl::Socket sock;
    ifioctl::set_interface_down(sock, get_name());
  }

  Physical::Physical(nl80211::wiphy wiphy) : m_index(wiphy.index),
    m_supported_cmds(wiphy.supported_cmds),
    m_supported_iftypes(wiphy.supported_iftypes) {}

  Physical::Physical(std::uint32_t index) : Physical(get_all_info(index)) {}

  nl80211::wiphy Physical::get_all_info(std::uint32_t index) {
    nl80211::Socket nlsock;
    return nl80211::commands::get_wiphy(nlsock, index);
  }

  std::string Physical::get_name() const {
    return get_all_info(m_index).name;
  }

  Virtual Physical::setup_streetpass_interface(std::string const& name) const {
    check_supported();
    auto virt_list = find_all_virtual();
    //TODO: exception handling
    for(auto virt: virt_list)
      virt.down();

    //TODO: exception handling
    nl80211::Socket nlsock;
    nl80211::wiface w = nl80211::commands::new_interface(nlsock, m_index, NL80211_IFTYPE_ADHOC, name);
    Virtual virt(w.index);
    std::this_thread::sleep_for (std::chrono::seconds(1));
    virt.down();
    nl80211::commands::set_interface_mode(nlsock, virt.get_id(), NL80211_IFTYPE_ADHOC);
    std::this_thread::sleep_for (std::chrono::seconds(1));
    virt.up();
    nl80211::commands::join_ibss(nlsock, virt.get_id(), "Nintendo_3DS_continuous_scan_000", 2412, true, virt.get_mac_addr());
    //TODO: implement!
    return virt;
  }

  bool Physical::is_supported() const noexcept {
    try {
      check_supported();
    } catch(...) {
      return false;
    }
    return true;
  }

  void Physical::check_supported() const {
    std::vector<std::uint32_t> const& types = m_supported_iftypes;
    auto type_it = std::find(types.begin(), types.end(), NL80211_IFTYPE_ADHOC);
    if(type_it == types.end())
      throw UnsupportedPhysicalInterface("Interface does not support adhoc mode");

    std::vector<std::uint32_t> const& cmds = m_supported_cmds;
    auto cmd_it = std::find(cmds.begin(), cmds.end(), NL80211_CMD_NEW_INTERFACE);
    if(cmd_it == cmds.end())
      throw UnsupportedPhysicalInterface("Interface does not support creating new virtual interfaces");

    cmd_it = std::find(cmds.begin(), cmds.end(), NL80211_CMD_NEW_KEY);
    if(cmd_it == cmds.end())
      throw UnsupportedPhysicalInterface("Interface does not support adding new crypto keys");

    cmd_it = std::find(cmds.begin(), cmds.end(), NL80211_CMD_JOIN_IBSS);
    if(cmd_it == cmds.end())
      throw UnsupportedPhysicalInterface("Interface does not support joining IBSS");

    cmd_it = std::find(cmds.begin(), cmds.end(), NL80211_CMD_FRAME);
    if(cmd_it == cmds.end())
      throw UnsupportedPhysicalInterface("Interface does not support sending custom frames");
  }

  std::vector<Virtual> Physical::find_all_virtual() const {
    std::vector<Virtual> res;

    nl80211::Socket nlsock;
    std::vector<nl80211::wiface> wifaces = nl80211::commands::get_interface_list(nlsock, m_index);
    std::transform(wifaces.begin(), wifaces.end(), std::back_inserter(res),
      [](auto x) { return Virtual(x.index); });

    return res;
  }

  std::vector<Physical> Physical::find_all() {
    std::vector<Physical> res;

    nl80211::Socket nlsock;
    std::vector<nl80211::wiphy> wiphys = nl80211::commands::get_wiphy_list(nlsock);
    std::transform(wiphys.begin(), wiphys.end(), std::back_inserter(res),
      [](auto x) { return Physical(x); });

    return res;
  }

  std::vector<Physical> Physical::find_all_supported() {
    std::vector<Physical> res = find_all();
    res.erase(std::remove_if(res.begin(), res.end(),
      [](auto x) { return !x.is_supported(); }), res.end());

    return res;
  }

  UnsupportedPhysicalInterface::UnsupportedPhysicalInterface(std::string const& msg) : m_msg(msg) {}

  const char* UnsupportedPhysicalInterface::what() const noexcept {
    return m_msg.c_str();
  }
}
