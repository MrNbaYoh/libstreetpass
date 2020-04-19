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
  class Message;

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
    void send_message(Message& msg);
    void recv_messages();
  };
}