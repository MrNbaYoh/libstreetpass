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

}
