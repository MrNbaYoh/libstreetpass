#pragma once

#include <cstdint>
#include <vector>

#include "nl80211/socket.hpp"

namespace nl80211::commands {

  void new_key(Socket& nlsock, std::uint32_t if_idx, std::uint8_t key_idx,
    std::uint32_t cipher, std::array<std::uint8_t, 6> const& mac,
    std::vector<std::uint8_t> const& key);

  void del_key(Socket& nlsock, std::uint32_t if_idx, std::uint8_t key_idx,
    std::array<std::uint8_t, 6> const& mac);

  void set_interface_mode(Socket& nlsock, std::uint32_t if_idx,
    nl80211_iftype mode);

  void register_frame(Socket& nlsock, std::uint32_t if_idx, std::uint16_t type,
    std::vector<std::uint8_t> const& match = {});

  void join_ibss(Socket& nlsock, std::uint32_t if_idx, std::string const& ssid,
    std::uint32_t freq, bool fixed_freq, std::array<std::uint8_t, 6> const& bssid);

  std::uint32_t new_interface(Socket& nlsock, std::uint32_t wiphy, nl80211_iftype type,
    std::string const& name);

  void del_interface(Socket& nlsock, std::uint32_t if_idx);

  struct wiphy {
    std::uint32_t index;
    std::string name;
    std::vector<std::uint32_t> supported_cmds;
    std::vector<std::uint32_t> supported_iftypes;
  };
  wiphy get_wiphy(Socket& nlsock, std::uint32_t wiphy);
  std::vector<wiphy> get_wiphy_list(Socket& nlsock);

  struct wiface {
    std::uint32_t wiphy;
    std::uint32_t index;
    std::uint32_t type;
    std::string name;
  };
  std::vector<wiface> get_interface_list(Socket& nlsock, std::uint32_t wiphy);
}
