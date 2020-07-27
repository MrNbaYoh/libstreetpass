#include "iface/physical.hpp"

#include <algorithm>
#include <iostream>

#include "iface/streetpass.hpp"
#include "iface/virtual.hpp"

namespace streetpass::iface {

PhysicalInterface::PhysicalInterface(nl80211::wiphy wiphy)
    : m_index(wiphy.index),
      m_supported_cmds(wiphy.supported_cmds),
      m_supported_iftypes(wiphy.supported_iftypes),
      m_supported_ciphers(wiphy.supported_ciphers) {}

PhysicalInterface::PhysicalInterface(std::uint32_t index)
    : PhysicalInterface(get_all_info(index)) {}

nl80211::wiphy PhysicalInterface::get_all_info(std::uint32_t index) {
  nl80211::Socket nlsock;
  return nl80211::commands::get_wiphy(nlsock, index);
}

std::string PhysicalInterface::get_name() const {
  return get_all_info(m_index).name;
}

StreetpassInterface PhysicalInterface::setup_streetpass_interface(
    std::string const& name) const {
  check_supported();
  auto virt_list = find_all_virtual();
  // TODO: exception handling
  for (auto virt : virt_list) virt.down();

  // TODO: implement!
  return StreetpassInterface(*this, name);
}

bool PhysicalInterface::is_supported() const noexcept {
  try {
    check_supported();
  } catch (...) {
    return false;
  }
  return true;
}

namespace {
struct command_requirement {
  std::uint32_t cmd;
  std::string error_msg;
};
std::vector<command_requirement> required_cmds = {
    {NL80211_CMD_NEW_INTERFACE,
     "Interface does not support creating new virtual interfaces"},
    {NL80211_CMD_NEW_KEY, "Interface does not support adding new crypto keys"},
    {NL80211_CMD_JOIN_IBSS, "Interface does not support joining IBSS"},
    {NL80211_CMD_FRAME, "Interface does not support sending custom frames"}};
constexpr std::uint32_t CIPHER_CCMP_128 = 0x000fac04;
}  // namespace

void PhysicalInterface::check_supported() const {
  auto type_it = m_supported_iftypes.find(NL80211_IFTYPE_ADHOC);
  if (type_it == m_supported_iftypes.end())
    throw UnsupportedPhysicalInterface("Interface does not support adhoc mode");

  for (const auto& req : required_cmds) {
    auto cmd_it = m_supported_cmds.find(req.cmd);
    if (cmd_it == m_supported_cmds.end())
      throw UnsupportedPhysicalInterface(req.error_msg);
  }

  auto cipher_it = m_supported_ciphers.find(CIPHER_CCMP_128);
  if (cipher_it == m_supported_ciphers.end())
    throw UnsupportedPhysicalInterface(
        "Interface does not support AES-CCMP-128 cipher");
}

std::vector<VirtualInterface> PhysicalInterface::find_all_virtual() const {
  std::vector<VirtualInterface> res;

  nl80211::Socket nlsock;
  std::vector<nl80211::wiface> wifaces =
      nl80211::commands::get_interface_list(nlsock, m_index);
  std::transform(wifaces.begin(), wifaces.end(), std::back_inserter(res),
                 [](auto x) { return VirtualInterface(x.index); });

  return res;
}

std::vector<PhysicalInterface> PhysicalInterface::find_all() {
  std::vector<PhysicalInterface> res;

  nl80211::Socket nlsock;
  std::vector<nl80211::wiphy> wiphys =
      nl80211::commands::get_wiphy_list(nlsock);
  std::transform(wiphys.begin(), wiphys.end(), std::back_inserter(res),
                 [](auto x) { return PhysicalInterface(x); });

  return res;
}

std::vector<PhysicalInterface> PhysicalInterface::find_all_supported() {
  std::vector<PhysicalInterface> res = find_all();
  res.erase(std::remove_if(res.begin(), res.end(),
                           [](auto x) { return !x.is_supported(); }),
            res.end());

  return res;
}

UnsupportedPhysicalInterface::UnsupportedPhysicalInterface(
    std::string const& msg)
    : m_msg(msg) {}

const char* UnsupportedPhysicalInterface::what() const noexcept {
  return m_msg.c_str();
}
}  // namespace streetpass::iface
