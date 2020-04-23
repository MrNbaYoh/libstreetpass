#include "nl80211/socket.hpp"
#include "nl80211/message.hpp"

#include "nl80211/error.hpp"

namespace streetpass::nl80211 {

  Socket::Socket() : m_nlsock(nl_socket_alloc(), nl_socket_free) {
    if(m_nlsock.get() == nullptr) {
      throw std::bad_alloc();
    }

    int res = genl_connect(m_nlsock.get());
    if(res < 0) {
      throw NlError(res, "Failed to connect socket");
    }

    m_driver_id = genl_ctrl_resolve(m_nlsock.get(), "nl80211");
    if(m_driver_id < 0) {
      throw NlError(res, "Failed to resolve nl80211 family");
    }
  }

  int Socket::get_driver_id() const {
    return m_driver_id;
  }

  void Socket::send_message(Message& msg) {
    int ret;
    try {
      ret = nl_send_auto(m_nlsock.get(), msg.m_nl_msg.get());
    } catch(...) {
      std::cerr << "Caught an exception while sending netlink messages! "
      "Memory leak in sight... aborting." << std::endl;
      std::exit(EXIT_FAILURE);
    }
  	if(ret < 0)
      throw NlError(ret, "Failed to send message");
  }

  void Socket::recv_messages() {
    int ret;
    try {
      ret = nl_recvmsgs_default(m_nlsock.get());
    } catch(...) {
      std::cerr << "Caught an exception while receiving netlink messages! "
      "Memory leak in sight... aborting." << std::endl;
      std::exit(EXIT_FAILURE);
    }
    if(ret < 0)
      throw NlError(ret, "An error occured while receiving messages");
  }

  void Socket::recv_messages(std::function<int(MessageParser&, void*)> callback, void* arg) {
    nl_cb* cb = nl_cb_alloc(NL_CB_DEFAULT);
    if(cb == nullptr)
      throw std::bad_alloc();

    auto noop_seq_check = [](nl_msg*, void*) -> int {
      return NL_OK;
    };

    nl_cb_set(cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, noop_seq_check, NULL);

    auto recv_msg = [callback, arg](nl_msg *nlmsg) {
      MessageParser msg(nlmsg);
      return callback(msg, arg);
    };

    auto recv_msg_cb = [](nl_msg* nlmsg, void* arg) {
      return (*static_cast<decltype(recv_msg)*>(arg))(nlmsg);
    };

    nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, recv_msg_cb, &recv_msg);

    int ret;
    try {
      ret = nl_recvmsgs(m_nlsock.get(), cb);
    } catch(...) {
      std::cerr << "Caught an exception while receiving netlink messages! "
      "Memory leak in sight... aborting." << std::endl;
      std::exit(EXIT_FAILURE);
    }
    if(ret < 0)
      throw NlError(ret, "An error occured while receiving messages");

    nl_cb_put(cb);
  }
}
