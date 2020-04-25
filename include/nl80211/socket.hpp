#pragma once
#include <exception>
#include <memory>
#include <functional>
#include <array>
#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <linux/nl80211.h>

namespace streetpass::nl80211 {
  class Message;
  class MessageParser;

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
    void recv_messages(std::function<void(MessageParser&, void*)> callback,
      void* arg, bool disable_seq_check = false);
  };
}
