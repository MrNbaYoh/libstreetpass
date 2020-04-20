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
    int res = nla_put_string(m_nl_msg.get(), attr, s.c_str());
    if(res < 0)
      //TODO: better exception
      throw "nla_put";
  }

  Message::Message(nl80211_commands cmd, int driver_id) :
    m_nl_msg(nlmsg_alloc(), nlmsg_free)
  {
    if(m_nl_msg.get() == nullptr)
      throw std::bad_alloc();

    void* p_res = genlmsg_put(m_nl_msg.get(), 0, 0, driver_id, 0, 0, cmd, 0);
    if(p_res == nullptr)
      //TODO: better exception
      throw "genlmsg_put";
  }

  MessageParser::MessageParser(nl_msg* nlmsg) {
    genlmsghdr *gnlh = static_cast<genlmsghdr *>(nlmsg_data(nlmsg_hdr(nlmsg)));
    if(gnlh == nullptr)
      //TODO: better exception
      throw "gnlh is null";

    cmd = gnlh->cmd;
    int ret = nla_parse(m_tb_msg.data(), NL80211_ATTR_MAX,
      genlmsg_attrdata(gnlh, 0), genlmsg_attrlen(gnlh, 0), NULL);
    if(ret < 0)
      //TODO: better exception
      throw "nla_parse";
  }

  std::uint8_t MessageParser::get_command() const {
    return cmd;
  }

  void MessageParser::get(nl80211_attrs attr, std::vector<std::uint8_t>& data) const {
    int len = nla_len(m_tb_msg.at(attr));
		std::uint8_t* d =
      static_cast<std::uint8_t*>(nla_data(m_tb_msg.at(attr)));
    data.insert(data.end(), d, d + len);
  }

  void MessageParser::get(nl80211_attrs attr, std::string& str) const {
		char* s = nla_get_string(m_tb_msg.at(attr));
    str.insert(str.size(), s);
  }

  void MessageParser::get(nl80211_attrs attr, std::uint32_t& w) const {
    w = nla_get_u32(m_tb_msg.at(attr));
  }

  void MessageParser::get(nl80211_attrs attr, std::uint16_t& w) const {
    w = nla_get_u16(m_tb_msg.at(attr));
  }

  void MessageParser::get(nl80211_attrs attr, std::uint8_t& w) const {
    w = nla_get_u8(m_tb_msg.at(attr));
  }

  void MessageParser::get(nl80211_attrs attr, bool& b) const {
    b = nla_get_flag(m_tb_msg.at(attr));
  }
}
