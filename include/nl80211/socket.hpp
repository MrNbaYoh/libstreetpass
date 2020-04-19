#pragma once
#include <exception>
#include <memory>
#include <functional>
#include <array>
#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <linux/nl80211.h>

namespace nl80211 {
  class Socket {
  private:
    std::unique_ptr<nl_sock, decltype(&nl_socket_free)> m_nlsock;
    int m_driver_id;
  public:
    Socket();
    ~Socket() = default;

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    Socket(Socket&&) = delete;
    Socket& operator=(Socket&&) = delete;

    int get_driver_id() const;

    void new_key(std::uint32_t if_idx, std::uint8_t key_idx, std::uint32_t cipher,
      std::array<std::uint8_t, 6> const& mac, std::vector<std::uint8_t> const& key);
    void del_key(std::uint32_t if_idx, std::uint8_t key_idx,
      std::array<std::uint8_t, 6> const& mac);
    void set_interface_mode(std::uint32_t if_idx, nl80211_iftype mode);
  };
}
