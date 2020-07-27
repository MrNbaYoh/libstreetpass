#pragma once
#include <linux/nl80211.h>
#include <netlink/genl/ctrl.h>
#include <netlink/genl/genl.h>
#include <netlink/netlink.h>

#include <array>
#include <exception>
#include <functional>
#include <memory>

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
                     void* arg, bool disable_seq_check = false,
                     unsigned int timeout = 0);
};
}  // namespace streetpass::nl80211
