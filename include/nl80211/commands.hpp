#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <netlink/genl/genl.h>
#include <linux/nl80211.h>

namespace nl80211::commands {
  void nla_put(nl_msg* msg, nl80211_attrs attr, std::uint32_t v);
  void nla_put(nl_msg* msg, nl80211_attrs attr, std::uint16_t v);
  void nla_put(nl_msg* msg, nl80211_attrs attr, std::uint8_t v);
  void nla_put(nl_msg* msg, nl80211_attrs attr);
  void nla_put(nl_msg* msg, nl80211_attrs attr, std::vector<std::uint8_t> v);
  void nla_put(nl_msg* msg, nl80211_attrs attr, std::string s);

  nl_msg* init_msg(nl80211_commands cmd, int driver_id, std::uint32_t if_idx);
  void send_and_free_msg(nl_sock* sock, nl_msg* msg);
}
