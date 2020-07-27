#pragma once

#include <tins/memory_helpers.h>

#include <array>
#include <cstdint>
#include <iostream>
#include <tuple>
#include <vector>

using Tins::Memory::InputMemoryStream;

namespace streetpass::cec {
using key_type = std::array<uint8_t, 8>;
using tid_type = uint32_t;
using bytes = std::vector<uint8_t>;

class ICecFormat {
 public:
  virtual bytes to_bytes() const = 0;
  virtual unsigned byte_size() const = 0;
};

template <class T>
class Parser {
  static_assert(std::is_base_of<ICecFormat, T>::value,
                "T should inherit from ICecFormat");

 public:
  static T from_stream(InputMemoryStream& stream) {
    return T::from_stream(stream);
  }

  static T from_bytes(const uint8_t* buffer, uint32_t size) {
    InputMemoryStream stream(buffer, size);
    return from_stream(stream);
  }

  static T from_bytes(bytes const& buffer) {
    return from_bytes(buffer.data(), buffer.size());
  }
};
}  // namespace streetpass::cec
