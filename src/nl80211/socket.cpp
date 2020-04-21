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

  void Socket::recv_messages(std::optional<std::function<int(MessageParser&, void*)>> opt_callback, std::function<void(int, void*)> err_callback, void* arg) {
    nl_cb* cb = nl_cb_alloc(NL_CB_DEFAULT);
    if(cb == nullptr)
      //TODO: better exception
      throw "nl_cb_alloc";

    auto noop_seq_check = [](nl_msg*, void*) -> int {
      return NL_OK;
    };

    auto err_msg = [err_callback, arg](int error) {
      err_callback(-error, arg);
      return -nl_syserr2nlerr(error);
    };

    auto err_msg_cb = [](sockaddr_nl*, nlmsgerr* nlerr, void* arg) {
      return (*static_cast<decltype(err_msg)*>(arg))(nlerr->error);
    };

    nl_cb_set(cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, noop_seq_check, NULL);
    nl_cb_err(cb, NL_CB_CUSTOM, err_msg_cb, &err_msg);

    if(opt_callback != std::nullopt) {
      auto recv_msg = [opt_callback, arg](nl_msg *nlmsg) {
        MessageParser msg(nlmsg);
        return opt_callback.value()(msg, arg);
      };

      auto recv_msg_cb = [](nl_msg* nlmsg, void* arg) {
        return (*static_cast<decltype(recv_msg)*>(arg))(nlmsg);
      };

      nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, recv_msg_cb, &recv_msg);
    }

    int ret = nl_recvmsgs(m_nlsock.get(), cb);
    if(ret < 0)
      //TODO: better exception
      throw "nl_recvmsgs";
    nl_cb_put(cb);
  }
}
