#pragma once
#include <exception>
#include <string>

namespace streetpass::nl80211 {
class NlError : public std::exception {
 private:
  int m_code;
  std::string m_msg;

 public:
  NlError(int code, std::string msg = "") noexcept;
  int code() const noexcept;
  const char* what() const noexcept;
  std::string message() const noexcept;
};
}  // namespace streetpass::nl80211
