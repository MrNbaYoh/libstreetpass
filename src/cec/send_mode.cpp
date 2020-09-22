#include "cec/send_mode.hpp"

#include <iomanip>

namespace streetpass::cec {
  bool SendMode::match(SendMode const& other) const {
    switch (m_value) {
      case EXCHANGE:
        return other == EXCHANGE;
      case RECV_ONLY:
        return other == SEND_ONLY ||
          other == SEND_RECV;
      case SEND_ONLY:
        return other == RECV_ONLY ||
          other == SEND_RECV;
      case SEND_RECV:
        return other != EXCHANGE;
      default:
        return false;
    }
  }

  SendMode::operator std::string() const {
    switch (m_value) {
      case EXCHANGE:
        return "EXCHANGE";
      case RECV_ONLY:
        return "RECV_ONLY";
      case SEND_ONLY:
        return "SEND_ONLY";
      case SEND_RECV:
        return "SEND_RECV";
      default:
        return "invalid";
    }
  }

  std::ostream& operator<<(std::ostream& s, const SendMode& m) {
    std::stringstream ss;
    ss << std::setfill('0');
    ss << std::hex << std::string(m) << "(" << std::setw(2) << m.m_value << ")";
    return s << ss.str();
  }
}  // namespace streetpass::cec
