#include "nl80211/commands.hpp"

#include <future>
#include <net/if.h>
#include "nl80211/message.hpp"

namespace streetpass::nl80211::commands {

  namespace {

    struct command_arg {
      std::exception_ptr e;
      void* arg;
    };

    int parse_wiphy_message(MessageParser& msg, void* arg) {
      auto cmd_arg = static_cast<struct command_arg*>(arg);
      auto w = static_cast<struct wiphy*>(cmd_arg->arg);

      try {
        w->index = msg.get<std::uint32_t>(NL80211_ATTR_WIPHY).value();
        w->name = msg.get<std::string>(NL80211_ATTR_WIPHY_NAME).value();

        auto cmd_attrs = msg.get<std::vector<MessageAttribute<std::uint32_t>>>(NL80211_ATTR_SUPPORTED_COMMANDS).value();
        for(auto attr: cmd_attrs)
          w->supported_cmds.push_back(attr.value());

        auto iftype_attrs = msg.get<std::vector<MessageAttribute<void>>>(NL80211_ATTR_SUPPORTED_IFTYPES).value();
        for(auto attr: iftype_attrs)
          w->supported_iftypes.push_back(attr.type());
      } catch(...) {
        cmd_arg->e = std::current_exception();
      }

      return NL_OK;
    };

  }

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

  void join_ibss(Socket& nlsock, std::uint32_t if_idx, std::string const& ssid,
    std::uint32_t freq, bool fixed_freq, std::array<std::uint8_t, 6> const& bssid)
  {
    if(ssid.size() > 0x20)
      throw std::invalid_argument("SSID is too long");

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
      throw std::invalid_argument("Interface name is too long");

    auto resp_handler = [](MessageParser& msg, void* arg) -> int {
      auto cmd_arg = static_cast<struct command_arg*>(arg);
      std::uint32_t* id = static_cast<std::uint32_t*>(cmd_arg->arg);

      try {
        *id = msg.get<std::uint32_t>(NL80211_ATTR_IFINDEX).value();
      } catch(...) {
        cmd_arg->e = std::current_exception();
      }

      return NL_OK;
    };

    Message msg(NL80211_CMD_NEW_INTERFACE, nlsock.get_driver_id());
    msg.put(NL80211_ATTR_WIPHY, wiphy);
    msg.put(NL80211_ATTR_IFTYPE, static_cast<std::uint32_t>(type));
    msg.put(NL80211_ATTR_IFNAME, name);
    nlsock.send_message(msg);

    std::uint32_t id;
    struct command_arg args = {};
    args.arg = &id;
    nlsock.recv_messages(resp_handler, &args);

    if(args.e)
      std::rethrow_exception(args.e);

    return id;
  }

  void del_interface(Socket& nlsock, std::uint32_t if_idx) {
    Message msg(NL80211_CMD_DEL_INTERFACE, nlsock.get_driver_id());
    msg.put(NL80211_ATTR_IFINDEX, if_idx);

    nlsock.send_message(msg);
    nlsock.recv_messages();
  }

  wiphy get_wiphy(Socket& nlsock, std::uint32_t wiphy) {
    Message msg(NL80211_CMD_GET_WIPHY, nlsock.get_driver_id());
    msg.put(NL80211_ATTR_WIPHY, wiphy);

    nlsock.send_message(msg);

    struct wiphy w = {};
    struct command_arg args = {};
    args.arg = &w;
    nlsock.recv_messages(parse_wiphy_message, &args);

    if(args.e)
      std::rethrow_exception(args.e);

    return w;
  }

  std::vector<wiphy> get_wiphy_list(Socket& nlsock) {
    auto resp_handler = [](MessageParser& msg, void* arg) -> int {
      auto cmd_arg = static_cast<struct command_arg*>(arg);
      auto v = static_cast<std::vector<struct wiphy>*>(cmd_arg->arg);

      try {
        auto index = msg.get<std::uint32_t>(NL80211_ATTR_WIPHY).value();;
        //if(!v->empty() && v->back().index == index)
        //  return NL_OK;

        struct wiphy w;
        struct command_arg args = {};
        args.arg = &w;
        parse_wiphy_message(msg, &args);
        if(args.e)
          std::rethrow_exception(args.e);

        v->push_back(w);
      } catch(...) {
        cmd_arg->e = std::current_exception();
      }

      return NL_OK;
    };
    Message msg(NL80211_CMD_GET_WIPHY, nlsock.get_driver_id(), NLM_F_DUMP);

    nlsock.send_message(msg);

    std::vector<struct wiphy> v;
    struct command_arg args = {};
    args.arg = &v;
    nlsock.recv_messages(resp_handler, &args);

    if(args.e)
      std::rethrow_exception(args.e);

    return v;
  }

  std::vector<wiface> get_interface_list(Socket& nlsock, std::uint32_t wiphy) {
    auto resp_handler = [](MessageParser& msg, void* arg) -> int {
      auto cmd_arg = static_cast<struct command_arg*>(arg);
      auto v = static_cast<std::vector<struct wiface>*>(cmd_arg->arg);

      try {
        auto index = msg.get<std::uint32_t>(NL80211_ATTR_IFINDEX).value();
        if(!v->empty() && v->back().index == index)
          return NL_OK;

        struct wiface i;
        i.index = index;
        i.wiphy = msg.get<std::uint32_t>(NL80211_ATTR_WIPHY).value();
        i.name = msg.get<std::string>(NL80211_ATTR_IFNAME).value();
        i.type = msg.get<std::uint32_t>(NL80211_ATTR_IFTYPE).value();

        auto mac = msg.get<std::vector<std::uint8_t>>(NL80211_ATTR_MAC).value();
        std::copy_n(mac.begin(), 6, i.mac.begin());

        v->push_back(i);
      } catch(...) {
        cmd_arg->e = std::current_exception();
      }

      return NL_OK;
    };
    Message msg(NL80211_CMD_GET_INTERFACE, nlsock.get_driver_id(), NLM_F_DUMP);
    msg.put(NL80211_ATTR_WIPHY, wiphy);

    nlsock.send_message(msg);

    std::vector<struct wiface> v;
    struct command_arg args = {};
    args.arg = &v;
    nlsock.recv_messages(resp_handler, &args);

    if(args.e)
      std::rethrow_exception(args.e);

    return v;
  }
}
