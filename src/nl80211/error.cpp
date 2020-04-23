#include "nl80211/error.hpp"

#include <netlink/netlink.h>

namespace streetpass::nl80211 {

  NlError::NlError(int code, std::string msg) noexcept : m_code(code),
    m_msg(msg) {}

  int NlError::code() const noexcept {
    return abs(m_code);
  }

  const char* NlError::what() const noexcept {
    return nl_geterror(m_code);
  }

  std::string NlError::message() const noexcept {
    return m_msg;
  }
}
