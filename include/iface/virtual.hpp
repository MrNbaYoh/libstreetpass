#pragma once

#include <array>
#include <cstdint>
#include <string>

#include "iface/physical.hpp"

namespace streetpass::iface {
  class VirtualInterface {
  protected:
    std::uint32_t m_index;
    VirtualInterface();
    static nl80211::wiface get_all_info(std::uint32_t index);
  private:
    VirtualInterface(std::uint32_t index);
    friend class PhysicalInterface;
  public:
    inline std::uint32_t get_id() const noexcept {
      return m_index;
    }

    std::array<std::uint8_t, 6> get_mac_addr() const;
    std::string get_name() const;

    void up() const;
    void down() const;
  };
}
