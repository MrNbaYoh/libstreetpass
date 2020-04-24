#include "nl80211/message.hpp"
#include "nl80211/error.hpp"

namespace streetpass::nl80211 {

  void Message::put(nl80211_attrs attr, std::uint32_t v) {
    int res = nla_put_u32(m_nl_msg.get(), attr, v);
    if(res < 0)
      throw NlError(res, "Failed to add attribute to message");
  }

  void Message::put(nl80211_attrs attr, std::uint16_t v) {
    int res = nla_put_u16(m_nl_msg.get(), attr, v);
    if(res < 0)
      throw NlError(res, "Failed to add attribute to message");
  }

  void Message::put(nl80211_attrs attr, std::uint8_t v) {
    int res = nla_put_u8(m_nl_msg.get(), attr, v);
    if(res < 0)
      throw NlError(res, "Failed to add attribute to message");
  }

  void Message::put(nl80211_attrs attr) {
    int res = nla_put_flag(m_nl_msg.get(), attr);
    if(res < 0)
      throw NlError(res, "Failed to add attribute to message");
  }

  void Message::put(nl80211_attrs attr, std::vector<std::uint8_t> const& v) {
    int res = nla_put(m_nl_msg.get(), attr, v.size(), v.data());
    if(res < 0)
      throw NlError(res, "Failed to add attribute to message");
  }

  void Message::put(nl80211_attrs attr, std::string const& s) {
    int res = nla_put_string(m_nl_msg.get(), attr, s.c_str());
    if(res < 0)
      throw NlError(res, "Failed to add attribute to message");
  }

  Message::Message(nl80211_commands cmd, int driver_id, int flags) :
    m_nl_msg(nlmsg_alloc(), nlmsg_free)
  {
    if(m_nl_msg.get() == nullptr)
      throw std::bad_alloc();

    void* p_res = genlmsg_put(m_nl_msg.get(), 0, 0, driver_id, 0, flags, cmd, 0);
    if(p_res == nullptr)
      throw std::bad_alloc();
  }

  MessageParser::MessageParser(nl_msg* nlmsg) {
    genlmsghdr *gnlh = static_cast<genlmsghdr *>(nlmsg_data(nlmsg_hdr(nlmsg)));
    if(gnlh == nullptr)
      throw std::invalid_argument("Message header is null");

    cmd = gnlh->cmd;
    int ret = nla_parse(m_tb_msg.data(), NL80211_ATTR_MAX,
      genlmsg_attrdata(gnlh, 0), genlmsg_attrlen(gnlh, 0), NULL);
    if(ret < 0)
      throw NlError(ret, "Failed to parse message");
  }

  std::uint8_t MessageParser::get_command() const {
    return cmd;
  }

  template<typename T>
  MessageAttribute<T>::MessageAttribute(nlattr* attr) {
    if(attr == nullptr)
      throw std::invalid_argument("Argument is null");

    m_type = nla_type(attr);
    m_len = nla_len(attr);

    load_content(attr);
  }

  MessageAttribute<void>::MessageAttribute(nlattr* attr) {
    if(attr == nullptr)
      throw std::invalid_argument("Argument is null");

    m_type = nla_type(attr);
    m_len = nla_len(attr);
  }

  template<typename T>
  void MessageAttribute<T>::load_content(nlattr* attr) {
    T* data = static_cast<T*>(nla_data(attr));
    if(m_len < sizeof(T))
      throw std::invalid_argument("Type length is greater than attribute length");

    m_content = *data;
  }

  template<>
  void MessageAttribute<std::string>::load_content(nlattr* attr) {
    char* data = nla_get_string(attr);
    m_content = std::string(data, m_len);
  }

  template<>
  void MessageAttribute<std::vector<std::uint8_t>>::load_content(nlattr* attr) {
    std::uint8_t* data = static_cast<std::uint8_t*>(nla_data(attr));
    m_content = std::vector<std::uint8_t>(data, data + m_len);
  }

  template<>
  void MessageAttribute<bool>::load_content(nlattr* attr) {
    m_content = nla_get_flag(attr);
  }
}
