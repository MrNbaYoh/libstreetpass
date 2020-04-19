#include "nl80211/message.hpp"

namespace nl80211 {

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

  MessageParser::MessageParser(nl_msg* nlmsg) {
    genlmsghdr *gnlh = static_cast<genlmsghdr *>(nlmsg_data(nlmsg_hdr(nlmsg)));
    if(gnlh == nullptr)
      //TODO: better exception
      throw "gnlh is null";

    int ret = nla_parse(m_tb_msg.data(), NL80211_ATTR_MAX,
      genlmsg_attrdata(gnlh, 0), genlmsg_attrlen(gnlh, 0), NULL);
    if(ret < 0)
      //TODO: better exception
      throw "nla_parse";
  }

  std::vector<std::uint8_t> MessageParser::get(nl80211_attrs attr) const {
    if(m_tb_msg.at(NL80211_ATTR_FRAME) == nullptr)
      return std::vector<std::uint8_t>();

    int len = nla_len(m_tb_msg.at(NL80211_ATTR_FRAME));
		std::uint8_t* data =
      static_cast<std::uint8_t*>(nla_data(m_tb_msg.at(NL80211_ATTR_FRAME)));
    return std::vector<std::uint8_t>(data, data + len);
  }
}
