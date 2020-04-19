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
    std::vector<std::uint8_t> const& match);
  void register_frame(Socket& nlsock, std::uint32_t if_idx, std::uint16_t type);
  void join_ibss(Socket& nlsock, std::uint32_t if_idx, std::string const& ssid,
    std::uint32_t freq, bool fixed_freq, std::array<std::uint8_t, 6> const& bssid);

}
