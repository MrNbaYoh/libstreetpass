#pragma once

#include <array>
#include <string>
#include <cstdint>

namespace crypto {
  void load_normal_key(std::string const& filepath);
  void load_normal_key(std::array<std::uint8_t, 16> const& key);

  void load_cecd_key(std::string const& filepath);
  void load_cecd_key(std::array<std::uint8_t, 17> const& key);

  std::array<std::uint8_t, 16> streetpass_ccmp_key(
    std::array<std::uint8_t, 8> master_cid,
    std::array<std::uint8_t, 6> master_mac,
    std::array<std::uint8_t, 8> client_cid,
    std::array<std::uint8_t, 6> client_mac);
}
