#include "nl80211/commands.hpp"
#include "nl80211/message.hpp"

namespace nl80211::commands {

  void new_key(Socket& nlsock, std::uint32_t if_idx, std::uint8_t key_idx,
    std::uint32_t cipher, std::array<std::uint8_t, 6> const& mac,
    std::vector<std::uint8_t> const& key)
  {
    Message msg(NL80211_CMD_NEW_KEY, nlsock.get_driver_id(), if_idx);
    msg.put(NL80211_ATTR_KEY_DATA, key);
    msg.put(NL80211_ATTR_KEY_CIPHER, cipher);
    msg.put(NL80211_ATTR_MAC, std::vector<std::uint8_t>(mac.begin(), mac.end()));
    msg.put(NL80211_ATTR_KEY_TYPE, static_cast<std::uint32_t>(NL80211_KEYTYPE_PAIRWISE));
    msg.put(NL80211_ATTR_KEY_IDX, key_idx);
    nlsock.send_message(msg);
    nlsock.recv_messages();
  }

  void del_key(Socket& nlsock, std::uint32_t if_idx, std::uint8_t key_idx,
    std::array<std::uint8_t, 6> const& mac)
  {
    Message msg(NL80211_CMD_DEL_KEY, nlsock.get_driver_id(), if_idx);
    msg.put(NL80211_ATTR_MAC, std::vector<std::uint8_t>(mac.begin(), mac.end()));
    msg.put(NL80211_ATTR_KEY_IDX, key_idx);
    nlsock.send_message(msg);
    nlsock.recv_messages();
  }

  void set_interface_mode(Socket& nlsock, std::uint32_t if_idx, nl80211_iftype mode) {
    Message msg(NL80211_CMD_SET_INTERFACE, nlsock.get_driver_id(), if_idx);
	  msg.put(NL80211_ATTR_IFTYPE, static_cast<std::uint32_t>(mode));
    nlsock.send_message(msg);
    nlsock.recv_messages();
  }

  void register_frame(Socket& nlsock, std::uint32_t if_idx, std::uint16_t type,
    std::vector<std::uint8_t> const& match)
  {
    Message msg(NL80211_CMD_REGISTER_FRAME, nlsock.get_driver_id(), if_idx);
    msg.put(NL80211_ATTR_FRAME_TYPE, type);
    msg.put(NL80211_ATTR_FRAME_MATCH, match);
    nlsock.send_message(msg);
    nlsock.recv_messages();
  }

  void register_frame(Socket& nlsock, std::uint32_t if_idx, std::uint16_t type)
  {
    register_frame(nlsock, if_idx, type, std::vector<std::uint8_t>());
  }

  void join_ibss(Socket& nlsock, std::uint32_t if_idx, std::string const& ssid,
    std::uint32_t freq, bool fixed_freq, std::array<std::uint8_t, 6> const& bssid)
  {
    if(ssid.size() > 0x20)
      throw "ssid too long";

    Message msg(NL80211_CMD_JOIN_IBSS, nlsock.get_driver_id(), if_idx);
    msg.put(NL80211_ATTR_SSID, ssid);
    msg.put(NL80211_ATTR_WIPHY_FREQ, freq);
    msg.put(NL80211_ATTR_MAC, std::vector<std::uint8_t>(bssid.begin(), bssid.end()));
    if(fixed_freq)
      msg.put(NL80211_ATTR_FREQ_FIXED);

    nlsock.send_message(msg);
    nlsock.recv_messages();
  }
}
