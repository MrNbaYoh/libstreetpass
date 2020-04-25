#pragma once

#include <vector>
#include <exception>
#include "nl80211/commands.hpp"

namespace streetpass::iface {

  class Virtual {
  private:
    std::uint32_t m_index;

    Virtual(std::uint32_t index);
    friend class Physical;
    static nl80211::wiface get_all_info(std::uint32_t index);
  public:
    inline std::uint32_t get_id() const noexcept {
      return m_index;
    }

    std::array<std::uint8_t, 6> get_mac_addr() const;
    std::string get_name() const;

    void up() const;
    void down() const;
  };

  class Physical {
  private:
    std::uint32_t m_index;
    std::vector<std::uint32_t> m_supported_cmds;
    std::vector<std::uint32_t> m_supported_iftypes;

    Physical(nl80211::wiphy wiphy);
    static nl80211::wiphy get_all_info(std::uint32_t index);
  public:
    Physical(std::uint32_t index);

    inline std::uint32_t get_id() const noexcept {
      return m_index;
    }

    std::string get_name() const;

    std::vector<Virtual> find_all_virtual() const;
    bool is_supported() const noexcept;
    void check_supported() const;
    Virtual setup_streetpass_interface(std::string const& name = "streetpass") const;

    static std::vector<Physical> find_all();
    static std::vector<Physical> find_all_supported();
  };


  class UnsupportedPhysicalInterface : public std::exception {
  private:
    std::string m_msg;
  public:
    UnsupportedPhysicalInterface(std::string const& msg);

    const char* what() const noexcept;
  };
}
