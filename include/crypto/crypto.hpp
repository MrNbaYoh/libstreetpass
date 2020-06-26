#pragma once

#include <array>
#include <string>
#include <cstdint>

namespace streetpass::crypto {
  void load_normal_key(std::string const& filepath);
  void load_normal_key(std::array<std::uint8_t, 16> const& key);

  void load_cecd_key(std::string const& filepath);
  void load_cecd_key(std::array<std::uint8_t, 17> const& key);

  std::array<std::uint8_t, 16> streetpass_ccmp_key(
    std::array<std::uint8_t, 8> const& master_key,
    std::array<std::uint8_t, 6> const& master_mac,
    std::array<std::uint8_t, 8> const& client_key,
    std::array<std::uint8_t, 6> const& client_mac);
}
