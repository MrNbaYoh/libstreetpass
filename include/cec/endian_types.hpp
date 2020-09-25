#pragma once

#include <tins/endianness.h>

using namespace Tins::Endian;

namespace streetpass::cec::endian_types {

enum endianness { LITTLE, BIG };

template <typename T, endianness E>
class EndianType {
 public:
  constexpr EndianType(T t) {
    if constexpr (E == LITTLE) {
      m_value = host_to_le(t);
    } else {
      m_value = host_to_be(t);
    }
  };

  constexpr operator T() const {
    if constexpr (E == LITTLE) {
      return le_to_host(m_value);
    } else {
      return be_to_host(m_value);
    }
  };

 private:
  T m_value;
};

using u8 = uint8_t;

using u16le = EndianType<uint16_t, LITTLE>;
using u32le = EndianType<uint32_t, LITTLE>;
using u64le = EndianType<uint64_t, LITTLE>;

using u16be = EndianType<uint16_t, BIG>;
using u32be = EndianType<uint32_t, BIG>;
using u64be = EndianType<uint64_t, BIG>;

using i8 = int8_t;

using i16le = EndianType<int16_t, LITTLE>;
using i32le = EndianType<int32_t, LITTLE>;
using i64le = EndianType<int64_t, LITTLE>;

using i16be = EndianType<int16_t, BIG>;
using i32be = EndianType<int32_t, BIG>;
using i64be = EndianType<int64_t, BIG>;
}  // namespace streetpass::cec::endian_types
