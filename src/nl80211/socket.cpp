#include "nl80211/socket.hpp"
#include "nl80211/message.hpp"

namespace nl80211 {

  Socket::Socket() : m_nlsock(nl_socket_alloc(), nl_socket_free) {
    if(m_nlsock.get() == nullptr) {
      //TODO: better exception
      throw std::bad_alloc();
    }

    int res = genl_connect(m_nlsock.get());
    if(res < 0) {
      //TODO: better exception
      throw "genl_connect failed";
    }

    m_driver_id = genl_ctrl_resolve(m_nlsock.get(), "nl80211");
    if(m_driver_id < 0) {
      //TODO: better exception
      throw "genl_ctrl_resolve failed";
    }
  }

  int Socket::get_driver_id() const {
    return m_driver_id;
  }

  void Socket::send_message(Message& msg) {
    int ret = nl_send_auto(m_nlsock.get(), msg.m_nl_msg.get());
  	if(ret < 0)
      //TODO: better exception
      throw "nl_send_auto";
  }

  void Socket::recv_messages() {
    int ret = nl_recvmsgs_default(m_nlsock.get());
    if(ret < 0)
      //TODO: better exception
      throw "nl_recvmsgs_default";
  }

  int noop_seq_check(struct nl_msg *msg, void *arg) {
  	return NL_OK;
  }

  struct recv_msg_args {
    std::function<int(MessageParser&, void*)> func;
    void* arg;
  };

  int recv_msg_cb(nl_msg *nlmsg, void *arg) {
    struct recv_msg_args* args = static_cast<struct recv_msg_args*>(arg);
    MessageParser msg(nlmsg);
    return args->func(msg, args->arg);
  }

  void Socket::recv_messages(std::function<int(MessageParser&, void*)> func, void* arg) {
    nl_cb* cb = nl_cb_alloc(NL_CB_DEFAULT);
    if(cb == nullptr)
      //TODO: better exception
      throw "nl_cb_alloc";

    struct recv_msg_args args = {func, arg};

    nl_cb_set(cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, noop_seq_check, NULL);
    nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, recv_msg_cb, &args);
    int ret = nl_recvmsgs(m_nlsock.get(), cb);
    if(ret < 0)
      //TODO: better exception
      throw "nl_recvmsgs";
    nl_cb_put(cb);
  }
}
