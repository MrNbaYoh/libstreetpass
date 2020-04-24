#pragma once

#include <vector>
#include <exception>
#include "nl80211/commands.hpp"

namespace streetpass::iface {

  class Virtual {
  private:
    nl80211::wiface m_wiface;

    Virtual(nl80211::wiface wiface);
    friend class Physical;
  public:
    inline std::array<std::uint8_t, 6> get_mac_addr() const noexcept {
      return m_wiface.mac;
    }
    inline std::string get_name() const noexcept {
      return m_wiface.name;
    }
    inline std::uint32_t get_id() const noexcept {
      return m_wiface.index;
    }
    void up() const;
    void down() const;
  };

  class Physical {
  private:
    nl80211::wiphy m_wiphy;

    Physical(nl80211::wiphy wiphy);
  public:
    inline std::string get_name() const noexcept {
      return m_wiphy.name;
    }
    inline std::uint32_t get_id() const noexcept {
      return m_wiphy.index;
    }

    std::vector<Virtual> find_all_virtual() const;
    bool is_supported() const noexcept;
    void check_supported() const;
    Virtual setup_streetpass_interface() const;

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
