#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <netlink/genl/genl.h>
#include <linux/nl80211.h>

#include "nl80211/socket.hpp"

namespace nl80211 {
  class Message {
  private:
    std::unique_ptr<nl_msg, decltype(&nlmsg_free)> m_nl_msg;
    friend void Socket::send_message(Message&);
  public:
    Message(nl80211_commands cmd, int driver_id, std::uint32_t if_idx);

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

  class MessageParser {
  private:
    std::array<nlattr*, NL80211_ATTR_MAX + 1> m_tb_msg;

    void get(nl80211_attrs attr, std::uint32_t& w) const;
    void get(nl80211_attrs attr, std::uint16_t& w) const;
    void get(nl80211_attrs attr, std::uint8_t& w) const;
    void get(nl80211_attrs attr, std::vector<std::uint8_t>& data) const;
    void get(nl80211_attrs attr, std::string& str) const;
    void get(nl80211_attrs attr, bool& b) const;
  public:
    MessageParser(nl_msg* nlmsg);

    MessageParser(const MessageParser&) = delete;
    MessageParser& operator=(const MessageParser&) = delete;
    MessageParser(MessageParser&&) = delete;
    MessageParser& operator=(MessageParser&&) = delete;

    template<typename T>
    T get(nl80211_attrs attr) const {
      if(m_tb_msg.at(NL80211_ATTR_FRAME) == nullptr)
        //TODO: better exception
        throw "invalid attr";

      T t;
      get(attr, t);
      return t;
    }
  };
}
