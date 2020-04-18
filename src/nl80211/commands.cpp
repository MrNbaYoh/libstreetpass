#include "nl80211/commands.hpp"

namespace nl80211::commands {
  void msg_put(nl_msg* msg, nl80211_attrs attr, std::uint32_t v) {
    int res = nla_put_u32(msg, attr, v);
    if(res < 0)
      //TODO: better exception
      throw "nla_put";
  }

  void msg_put(nl_msg* msg, nl80211_attrs attr, std::uint16_t v) {
    int res = nla_put_u16(msg, attr, v);
    if(res < 0)
      //TODO: better exception
      throw "nla_put";
  }

  void msg_put(nl_msg* msg, nl80211_attrs attr, std::uint8_t v) {
    int res = nla_put_u8(msg, attr, v);
    if(res < 0)
      //TODO: better exception
      throw "nla_put";
  }

  void msg_put(nl_msg* msg, nl80211_attrs attr) {
    int res = nla_put_flag(msg, attr);
    if(res < 0)
      //TODO: better exception
      throw "nla_put";
  }

  void msg_put(nl_msg* msg, nl80211_attrs attr, std::vector<std::uint8_t> v) {
    int res = nla_put(msg, attr, v.size(), v.data());
    if(res < 0)
      //TODO: better exception
      throw "nla_put";
  }

  void msg_put(nl_msg* msg, nl80211_attrs attr, std::string s) {
    int res = nla_put(msg, attr, s.size(), s.c_str());
    if(res < 0)
      //TODO: better exception
      throw "nla_put";
  }

  nl_msg* msg_init(nl80211_commands cmd, int driver_id, std::uint32_t if_idx) {
    nl_msg* msg = nlmsg_alloc();
    if(msg == nullptr)
      throw std::bad_alloc();

    void* p_res = genlmsg_put(msg, 0, 0, driver_id, 0, 0, NL80211_CMD_JOIN_IBSS, 0);
    if(p_res == nullptr)
      //TODO: better exception
      throw "genlmsg_put";
    msg_put(msg, NL80211_ATTR_IFINDEX, if_idx);
  }

  void msg_send_free(nl_sock* sock, nl_msg* msg) {
    int ret = nl_send_auto(sock, msg);
  	if(ret < 0)
  	{
      nlmsg_free(msg);
      //TODO: better exception
      throw "nl_send_auto";
    }

  	nl_recvmsgs_default(sock);
  	nlmsg_free(msg);
  }
}
