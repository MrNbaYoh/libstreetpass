#include "nl80211/commands.hpp"

namespace nl80211::commands {

  void Message::put(nl80211_attrs attr, std::uint32_t v) {
    int res = nla_put_u32(m_nl_msg.get(), attr, v);
    if(res < 0)
      //TODO: better exception
      throw "nla_put";
  }

  void Message::put(nl80211_attrs attr, std::uint16_t v) {
    int res = nla_put_u16(m_nl_msg.get(), attr, v);
    if(res < 0)
      //TODO: better exception
      throw "nla_put";
  }

  void Message::put(nl80211_attrs attr, std::uint8_t v) {
    int res = nla_put_u8(m_nl_msg.get(), attr, v);
    if(res < 0)
      //TODO: better exception
      throw "nla_put";
  }

  void Message::put(nl80211_attrs attr) {
    int res = nla_put_flag(m_nl_msg.get(), attr);
    if(res < 0)
      //TODO: better exception
      throw "nla_put";
  }

  void Message::put(nl80211_attrs attr, std::vector<std::uint8_t> const& v) {
    int res = nla_put(m_nl_msg.get(), attr, v.size(), v.data());
    if(res < 0)
      //TODO: better exception
      throw "nla_put";
  }

  void Message::put(nl80211_attrs attr, std::string const& s) {
    int res = nla_put(m_nl_msg.get(), attr, s.size(), s.c_str());
    if(res < 0)
      //TODO: better exception
      throw "nla_put";
  }

  Message::Message(nl80211_commands cmd, int driver_id, std::uint32_t if_idx) :
    m_nl_msg(nlmsg_alloc(), nlmsg_free)
  {
    if(m_nl_msg.get() == nullptr)
      throw std::bad_alloc();

    void* p_res = genlmsg_put(m_nl_msg.get(), 0, 0, driver_id, 0, 0, cmd, 0);
    if(p_res == nullptr)
      //TODO: better exception
      throw "genlmsg_put";
    put(NL80211_ATTR_IFINDEX, if_idx);
  }

  void Message::send(nl_sock* sock) {
    int ret = nl_send_auto(sock, m_nl_msg.get());
  	if(ret < 0)
      //TODO: better exception
      throw "nl_send_auto";

  	nl_recvmsgs_default(sock);
  }
}
