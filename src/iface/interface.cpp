#include "iface/interface.hpp"
#include "utils/ifioctl.hpp"

#include <algorithm>

namespace streetpass::iface {

  Virtual::Virtual(nl80211::wiface wiface) : m_wiface(wiface) {}

  void Virtual::up() const {
    //TODO: exception handling?
    ifioctl::Socket sock;
    ifioctl::set_interface_up(sock, m_wiface.name);
  }

  void Virtual::down() const {
    //TODO: exception handling?
    ifioctl::Socket sock;
    ifioctl::set_interface_down(sock, m_wiface.name);
  }

  Physical::Physical(nl80211::wiphy wiphy) : m_wiphy(wiphy) {}

  Virtual Physical::setup_streetpass_interface() const {
    check_supported();
    auto virt_list = find_all_virtual();
    for(auto virt: virt_list)
      virt.down();
    //TODO: implement!
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
    std::vector<std::uint32_t> const& types = m_wiphy.supported_iftypes;
    auto type_it = std::find(types.begin(), types.end(), NL80211_IFTYPE_ADHOC);
    if(type_it == types.end())
      throw UnsupportedPhysicalInterface("Interface does not support adhoc mode");

    std::vector<std::uint32_t> const& cmds = m_wiphy.supported_cmds;
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
    std::vector<nl80211::wiface> wifaces = nl80211::commands::get_interface_list(nlsock, m_wiphy.index);
    std::transform(wifaces.begin(), wifaces.end(), std::back_inserter(res),
      [](auto x) { return Virtual(x); });

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
