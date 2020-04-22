#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <netlink/genl/genl.h>
#include <linux/nl80211.h>

#include "nl80211/socket.hpp"
#include <iostream>

namespace nl80211 {
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

  template<typename T>
  class MessageAttribute {
  private:
    std::uint16_t m_len;
    std::uint16_t m_type;
    T m_content;

    void load_content(nlattr* attr);
  public:
    MessageAttribute(nlattr* attr);

    inline std::uint16_t type() const {
      return m_type;
    }

    inline T value() const {
      return m_content;
    }
  };

  template class MessageAttribute<bool>;
  template class MessageAttribute<std::uint8_t>;
  template class MessageAttribute<std::uint16_t>;
  template class MessageAttribute<std::uint32_t>;
  template class MessageAttribute<std::string>;
  template class MessageAttribute<std::vector<std::uint8_t>>;

  template <>
  class MessageAttribute<void> {
  private:
    std::uint16_t m_len;
    std::uint16_t m_type;
  public:
    MessageAttribute(nlattr* attr);

    inline std::uint16_t type() const {
      return m_type;
    }
  };

  template<typename T>
  class MessageAttribute<std::vector<MessageAttribute<T>>> {
  private:

    std::uint16_t m_len;
    std::uint16_t m_type;
    std::vector<MessageAttribute<T>> m_content;

  public:
    MessageAttribute(nlattr* attr) {
      if(attr == nullptr)
        throw std::invalid_argument("Attribute is null");

      m_type = nla_type(attr);
      m_len = nla_len(attr);

      int rem;
      nlattr* nested;
      nla_for_each_nested(nested, attr, rem)
        m_content.push_back(MessageAttribute<T>(nested));
    }

    inline std::uint16_t type() const {
      return m_type;
    }

    inline std::vector<MessageAttribute<T>> value() const {
      return m_content;
    }
  };

  class MessageParser {
  private:
    std::array<nlattr*, NL80211_ATTR_MAX + 1> m_tb_msg;
    std::uint8_t cmd;

  public:
    MessageParser(nl_msg* nlmsg);

    MessageParser(const MessageParser&) = delete;
    MessageParser& operator=(const MessageParser&) = delete;
    MessageParser(MessageParser&&) = delete;
    MessageParser& operator=(MessageParser&&) = delete;

    std::uint8_t get_command() const;

    template<typename T>
    MessageAttribute<T> get(nl80211_attrs attr) const {
      if(m_tb_msg.at(attr) == nullptr)
        throw std::invalid_argument("Attribute is null");

      return MessageAttribute<T>(m_tb_msg.at(attr));
    }
  };
}
