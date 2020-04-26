#pragma once

#include <vector>
#include <unordered_set>
#include <exception>
#include "nl80211/commands.hpp"

namespace streetpass::iface {
  class StreetpassInterface;
  class VirtualInterface;

  class PhysicalInterface {
  private:
    std::uint32_t m_index;
    std::unordered_set<std::uint32_t> m_supported_cmds;
    std::unordered_set<std::uint32_t> m_supported_iftypes;
    std::unordered_set<std::uint32_t> m_supported_ciphers;

    PhysicalInterface(nl80211::wiphy wiphy);
    static nl80211::wiphy get_all_info(std::uint32_t index);
  public:
    PhysicalInterface(std::uint32_t index);

    inline std::uint32_t get_id() const noexcept {
      return m_index;
    }

    std::string get_name() const;

    std::vector<VirtualInterface> find_all_virtual() const;
    bool is_supported() const noexcept;
    void check_supported() const;
    StreetpassInterface setup_streetpass_interface(std::string const& name = "streetpass") const;

    static std::vector<PhysicalInterface> find_all();
    static std::vector<PhysicalInterface> find_all_supported();
  };


  class UnsupportedPhysicalInterface : public std::exception {
  private:
    std::string m_msg;
  public:
    UnsupportedPhysicalInterface(std::string const& msg);

    const char* what() const noexcept;
  };
}
