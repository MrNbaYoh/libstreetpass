#pragma once

#include <linux/nl80211.h>
#include <netlink/genl/genl.h>

#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "nl80211/socket.hpp"

namespace streetpass::nl80211 {
class Message {
 private:
  std::unique_ptr<nl_msg, decltype(&nlmsg_free)> m_nl_msg;
  friend void Socket::send_message(Message&);

 public:
  Message(nl80211_commands cmd, int driver_id, int flags = 0);

  Message(const Message&) = delete;
  Message& operator=(const Message&) = delete;
  Message(Message&&) = delete;
  Message& operator=(Message&&) = delete;

  void put(nl80211_attrs attr, std::uint32_t v);
  void put(nl80211_attrs attr, std::uint16_t v);
  void put(nl80211_attrs attr, std::uint8_t v);
  void put(nl80211_attrs attr);
  void put(nl80211_attrs attr, std::vector<std::uint8_t> const& v);
  void put(nl80211_attrs attr, std::string const& s);
};

template <typename T>
class Attribute {
 private:
  T m_content;
  std::uint16_t m_type;
  std::uint16_t m_len;

  void load_content(nlattr* attr);

 public:
  Attribute(nlattr* attr);

  // TODO: move/copy operator/constructors ?

  inline std::uint16_t type() const { return m_type; }

  inline T value() const { return m_content; }
};

template <>
class Attribute<void> {
 private:
  std::uint16_t m_len;
  std::uint16_t m_type;

 public:
  Attribute(nlattr* attr);

  inline std::uint16_t type() const { return m_type; }
};

class Attributes {
 public:
  std::map<int, nlattr*> m_attrs;
  std::vector<int> m_attr_types;

 public:
  Attributes(nl_msg* nlmsg);
  Attributes(nlattr* attr);

  // TODO: move/copy operator/constructors ?

  std::vector<int> const& types() const { return m_attr_types; }

  template <typename T>
  Attribute<T> get(int attr) const {
    nlattr* attr_ptr = nullptr;
    try {
      attr_ptr = m_attrs.at(attr);
    } catch (std::out_of_range&) {
      throw std::invalid_argument("Attribute not found!");
    }

    return Attribute<T>(attr_ptr);
  }
};
}  // namespace streetpass::nl80211
