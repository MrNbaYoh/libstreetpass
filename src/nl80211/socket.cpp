#include "nl80211/socket.hpp"

#include <chrono>

#include "nl80211/error.hpp"
#include "nl80211/message.hpp"

namespace streetpass::nl80211 {

Socket::Socket() : m_nlsock(nl_socket_alloc(), nl_socket_free) {
  if (m_nlsock.get() == nullptr) {
    throw std::bad_alloc();
  }

  int res = genl_connect(m_nlsock.get());
  if (res < 0) {
    throw NlError(res, "Failed to connect socket");
  }

  m_driver_id = genl_ctrl_resolve(m_nlsock.get(), "nl80211");
  if (m_driver_id < 0) {
    throw NlError(res, "Failed to resolve nl80211 family");
  }
}

int Socket::get_driver_id() const { return m_driver_id; }

void Socket::send_message(Message &msg) {
  int ret;
  try {
    ret = nl_send_auto(m_nlsock.get(), msg.m_nl_msg.get());
  } catch (...) {
    std::cerr << "Caught an exception while sending netlink messages! "
                 "Memory leak in sight... aborting."
              << std::endl;
    std::exit(EXIT_FAILURE);
  }
  if (ret < 0) throw NlError(ret, "Failed to send message");
}

namespace {
int finish_handler(nl_msg *, void *arg) {
  int *ret = static_cast<int *>(arg);
  *ret = 0;
  return NL_SKIP;
}

int ack_handler(nl_msg *, void *arg) {
  int *ret = static_cast<int *>(arg);
  *ret = 0;
  return NL_STOP;
}

int error_handler(sockaddr_nl *, nlmsgerr *err, void *arg) {
  int *ret = static_cast<int *>(arg);
  *ret = -nl_syserr2nlerr(err->error);
  return NL_STOP;
}
}  // namespace

void Socket::recv_messages() {
  nl_cb *cb = nl_cb_alloc(NL_CB_DEFAULT);
  if (cb == nullptr) throw std::bad_alloc();

  int err = 1;

  nl_cb_err(cb, NL_CB_CUSTOM, error_handler, &err);
  nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &err);
  nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &err);

  try {
    while (err > 0) nl_recvmsgs(m_nlsock.get(), cb);
  } catch (...) {
    std::cerr << "Caught an exception while receiving netlink messages! "
                 "Memory leak in sight... aborting."
              << std::endl;
    std::exit(EXIT_FAILURE);
  }

  nl_cb_put(cb);
  if (err < 0) throw NlError(err, "An error occured while receiving messages");
}

void Socket::recv_messages(
    std::function<void(MessageParser &, void *)> callback, void *arg,
    bool disable_seq_check, unsigned int timeout) {
  nl_cb *cb = nl_cb_alloc(NL_CB_DEFAULT);
  if (cb == nullptr) throw std::bad_alloc();

  std::exception_ptr ex;
  int err = 1;

  auto recv_msg_cb = [callback, arg, &ex, &err](nl_msg *nlmsg) -> int {
    MessageParser msg(nlmsg);
    try {
      callback(msg, arg);
    } catch (...) {
      ex = std::current_exception();
      err = -1;
      return NL_STOP;
    }
    return NL_OK;
  };

  auto valid_handler = [](nl_msg *nlmsg, void *arg) {
    return (*static_cast<decltype(recv_msg_cb) *>(arg))(nlmsg);
  };

  auto no_seq_check = [](nl_msg *, void *) -> int { return NL_OK; };

  nl_cb_err(cb, NL_CB_CUSTOM, error_handler, &err);
  nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &err);
  nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &err);
  if (disable_seq_check)
    nl_cb_set(cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, no_seq_check, nullptr);
  nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, valid_handler, &recv_msg_cb);

  auto start = std::chrono::system_clock::now();
  unsigned int elapsed_ms = 0;
  try {
    do {
      nl_recvmsgs(m_nlsock.get(), cb);
      auto now = std::chrono::system_clock::now();
      elapsed_ms =
          std::chrono::duration_cast<std::chrono::milliseconds>(now - start)
              .count();
    } while (err > 0 && (elapsed_ms < timeout || !timeout));
  } catch (...) {
    std::cerr << "Caught an exception while receiving netlink messages! "
                 "Memory leak in sight... aborting."
              << std::endl;
    std::exit(EXIT_FAILURE);
  }

  nl_cb_put(cb);
  if (ex) std::rethrow_exception(ex);
  if (err < 0) throw NlError(err, "An error occured while receiving messages");
}
}  // namespace streetpass::nl80211
