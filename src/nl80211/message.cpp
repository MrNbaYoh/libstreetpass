#include "nl80211/message.hpp"

#include "nl80211/error.hpp"

namespace streetpass::nl80211 {

void Message::put(nl80211_attrs attr, std::uint32_t v) {
  int res = nla_put_u32(m_nl_msg.get(), attr, v);
  if (res < 0) throw NlError(res, "Failed to add attribute to message");
}

void Message::put(nl80211_attrs attr, std::uint16_t v) {
  int res = nla_put_u16(m_nl_msg.get(), attr, v);
  if (res < 0) throw NlError(res, "Failed to add attribute to message");
}

void Message::put(nl80211_attrs attr, std::uint8_t v) {
  int res = nla_put_u8(m_nl_msg.get(), attr, v);
  if (res < 0) throw NlError(res, "Failed to add attribute to message");
}

void Message::put(nl80211_attrs attr) {
  int res = nla_put_flag(m_nl_msg.get(), attr);
  if (res < 0) throw NlError(res, "Failed to add attribute to message");
}

void Message::put(nl80211_attrs attr, std::vector<std::uint8_t> const& v) {
  int res = nla_put(m_nl_msg.get(), attr, v.size(), v.data());
  if (res < 0) throw NlError(res, "Failed to add attribute to message");
}

void Message::put(nl80211_attrs attr, std::string const& s) {
  int res = nla_put_string(m_nl_msg.get(), attr, s.c_str());
  if (res < 0) throw NlError(res, "Failed to add attribute to message");
}

Message::Message(nl80211_commands cmd, int driver_id, int flags)
    : m_nl_msg(nlmsg_alloc(), nlmsg_free) {
  if (m_nl_msg.get() == nullptr) throw std::bad_alloc();

  void* p_res = genlmsg_put(m_nl_msg.get(), 0, 0, driver_id, 0, flags, cmd, 0);
  if (p_res == nullptr) throw std::bad_alloc();
}

Attributes::Attributes(nl_msg* nlmsg) {
  if (nlmsg == nullptr) throw std::invalid_argument("Message pointer is null");
  genlmsghdr* gnlh = static_cast<genlmsghdr*>(nlmsg_data(nlmsg_hdr(nlmsg)));

  nlattr* current_attr = nullptr;
  int rem = 0;
  nla_for_each_attr(current_attr, genlmsg_attrdata(gnlh, 0),
                    genlmsg_attrlen(gnlh, 0), rem) {
    int attr_type = nla_type(current_attr);
    m_attrs[attr_type] = current_attr;
    m_attr_types.push_back(attr_type);
  }
}

Attributes::Attributes(nlattr* attr) {
  if (attr == nullptr) throw std::invalid_argument("Attribute pointer is null");
  nlattr* current_attr = nullptr;
  int rem = 0;
  nla_for_each_nested(current_attr, attr, rem) {
    int attr_type = nla_type(current_attr);
    m_attrs[attr_type] = current_attr;
    m_attr_types.push_back(attr_type);
  }
}

template <typename T>
Attribute<T>::Attribute(nlattr* attr) {
  if (attr == nullptr) throw std::invalid_argument("Argument is null");

  m_type = nla_type(attr);
  m_len = nla_len(attr);

  load_content(attr);
}

template <>
Attribute<Attributes>::Attribute(nlattr* attr)
    : m_content(Attributes(attr)),
      m_type(nla_type(attr)),
      m_len(nla_len(attr)) {}

Attribute<void>::Attribute(nlattr* attr) {
  if (attr == nullptr) throw std::invalid_argument("Argument is null");

  m_type = nla_type(attr);
  m_len = nla_len(attr);
}

template <typename T>
void Attribute<T>::load_content(nlattr* attr) {
  T* data = static_cast<T*>(nla_data(attr));
  if (m_len < sizeof(T))
    throw std::invalid_argument("Type length is greater than attribute length");

  m_content = *data;
}

template <>
void Attribute<std::string>::load_content(nlattr* attr) {
  char* data = nla_get_string(attr);
  m_content = std::string(data, m_len);
}

template <>
void Attribute<std::vector<std::uint8_t>>::load_content(nlattr* attr) {
  std::uint8_t* data = static_cast<std::uint8_t*>(nla_data(attr));
  m_content = std::vector<std::uint8_t>(data, data + m_len);
}

template <>
void Attribute<std::vector<std::uint16_t>>::load_content(nlattr* attr) {
  std::uint16_t* data = static_cast<std::uint16_t*>(nla_data(attr));
  m_content =
      std::vector<std::uint16_t>(data, data + m_len / sizeof(std::uint16_t));
}

template <>
void Attribute<std::vector<std::uint32_t>>::load_content(nlattr* attr) {
  std::uint32_t* data = static_cast<std::uint32_t*>(nla_data(attr));
  m_content =
      std::vector<std::uint32_t>(data, data + m_len / sizeof(std::uint32_t));
}

template <>
void Attribute<bool>::load_content(nlattr* attr) {
  m_content = nla_get_flag(attr);
}

template class Attribute<bool>;
template class Attribute<std::uint8_t>;
template class Attribute<std::uint16_t>;
template class Attribute<std::uint32_t>;
template class Attribute<std::string>;
template class Attribute<std::vector<std::uint8_t>>;
template class Attribute<std::vector<std::uint16_t>>;
template class Attribute<std::vector<std::uint32_t>>;

}  // namespace streetpass::nl80211
