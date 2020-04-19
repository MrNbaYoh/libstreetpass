#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <netlink/genl/genl.h>
#include <linux/nl80211.h>

namespace nl80211::commands {
  class Message {
  private:
    std::unique_ptr<nl_msg, decltype(&nlmsg_free)> m_nl_msg;
  public:
    Message(nl80211_commands cmd, int driver_id, std::uint32_t id_idx);

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

    void send(nl_sock* sock);
  };
}
