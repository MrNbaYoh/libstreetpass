#pragma once
#include <cstdint>

namespace endianness {

  constexpr std::uint16_t to_le(std::uint16_t w) {
    if constexpr (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
      return w;
    else
      return (((w & 0xFF) << 8) | (w >> 8));
  }

  constexpr std::uint32_t to_le(std::uint32_t w) {
    if constexpr (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
      return w;
    else
      return (((w & 0xFF) << 24) | ((w & 0xFF00) << 8) | ((w & 0xFF0000) >> 8) | (w >> 24));
  }

  constexpr std::uint64_t to_le(std::uint64_t w) {
    if constexpr (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
      return w;
    else
      return (std::uint64_t)to_le((std::uint32_t)w) << 32 | to_le((std::uint32_t)(w >> 32));
  }

}
