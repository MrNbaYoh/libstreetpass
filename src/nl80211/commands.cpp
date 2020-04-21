#include "nl80211/commands.hpp"

#include <future>
#include <net/if.h>
#include "nl80211/message.hpp"

namespace nl80211::commands {

  void basic_err_handler(int error, void* arg) {
    if(error == 0)
      return;
    throw std::system_error(
          std::error_code(error, std::generic_category())
        );
  };

  void new_key(Socket& nlsock, std::uint32_t if_idx, std::uint8_t key_idx,
    std::uint32_t cipher, std::array<std::uint8_t, 6> const& mac,
    std::vector<std::uint8_t> const& key)
  {
    Message msg(NL80211_CMD_NEW_KEY, nlsock.get_driver_id());
    msg.put(NL80211_ATTR_IFINDEX, if_idx);
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
    Message msg(NL80211_CMD_DEL_KEY, nlsock.get_driver_id());
    msg.put(NL80211_ATTR_IFINDEX, if_idx);
    msg.put(NL80211_ATTR_MAC, std::vector<std::uint8_t>(mac.begin(), mac.end()));
    msg.put(NL80211_ATTR_KEY_IDX, key_idx);
    nlsock.send_message(msg);
    nlsock.recv_messages();
  }

  void set_interface_mode(Socket& nlsock, std::uint32_t if_idx, nl80211_iftype mode) {
    Message msg(NL80211_CMD_SET_INTERFACE, nlsock.get_driver_id());
    msg.put(NL80211_ATTR_IFINDEX, if_idx);
	  msg.put(NL80211_ATTR_IFTYPE, static_cast<std::uint32_t>(mode));
    nlsock.send_message(msg);
    nlsock.recv_messages();
  }

  void register_frame(Socket& nlsock, std::uint32_t if_idx, std::uint16_t type,
    std::vector<std::uint8_t> const& match)
  {
    Message msg(NL80211_CMD_REGISTER_FRAME, nlsock.get_driver_id());
    msg.put(NL80211_ATTR_IFINDEX, if_idx);
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
      //TODO: better exception
      throw "ssid too long";

    Message msg(NL80211_CMD_JOIN_IBSS, nlsock.get_driver_id());
    msg.put(NL80211_ATTR_IFINDEX, if_idx);
    msg.put(NL80211_ATTR_SSID, ssid);
    msg.put(NL80211_ATTR_WIPHY_FREQ, freq);
    msg.put(NL80211_ATTR_MAC, std::vector<std::uint8_t>(bssid.begin(), bssid.end()));
    if(fixed_freq)
      msg.put(NL80211_ATTR_FREQ_FIXED);

    nlsock.send_message(msg);
    nlsock.recv_messages();
  }

  std::uint32_t new_interface(Socket& nlsock, std::uint32_t wiphy, nl80211_iftype type,
    std::string const& name)
  {
    if(name.size() > IFNAMSIZ - 1)
      //TODO: better exception
      throw "name too long";

    auto resp_handler = [](MessageParser& msg, void* arg) -> int {
      std::uint32_t* id = static_cast<std::uint32_t*>(arg);
      *id = msg.get<std::uint32_t>(NL80211_ATTR_IFINDEX);
      return NL_OK;
    };

    Message msg(NL80211_CMD_NEW_INTERFACE, nlsock.get_driver_id());
    msg.put(NL80211_ATTR_WIPHY, wiphy);
    msg.put(NL80211_ATTR_IFTYPE, static_cast<std::uint32_t>(type));
    msg.put(NL80211_ATTR_IFNAME, name);
    nlsock.send_message(msg);

    std::uint32_t id;
    nlsock.recv_messages(resp_handler, basic_err_handler, &id);

    return id;
  }

  void del_interface(Socket& nlsock, std::uint32_t if_idx) {
    Message msg(NL80211_CMD_DEL_INTERFACE, nlsock.get_driver_id());
    msg.put(NL80211_ATTR_IFINDEX, if_idx);

    nlsock.send_message(msg);
    nlsock.recv_messages();
  }
}
