#pragma once

#include "cec/cec.hpp"
#include <vector>
#include <array>
#include <cstdint>
#include <stdexcept>
#include <tins/memory_helpers.h>
#include <tins/small_uint.h>

using Tins::small_uint;
using Tins::Memory::InputMemoryStream;

namespace streetpass::cec {
  enum class filter_list_marker_t : uint8_t {
    TITLE_FILTER = 0x1,
    KEY_FILTER = 0xF
  };

  enum class send_mode_t : uint8_t {
    EXCHANGE = 0x0,
    RECV_ONLY = 0x1,
    SEND_ONLY = 0X2,
    SEND_RECV = 0x3,
  };

  class ModuleFilter : public ICecFormat {
  public:
    ModuleFilter(InputMemoryStream& stream);
    ModuleFilter(const uint8_t* buffer, uint32_t size);
    ModuleFilter(bytes const& buffer);

    bytes to_bytes() const;
    friend std::ostream& operator<<(std::ostream& s, const ModuleFilter& l);

    class Filter : public ICecFormat {
    public:
      virtual unsigned total_size() const = 0;
    };

    class TitleFilter : public Filter {
    public:
      struct title_filter_mve {
        uint8_t mask;
        uint8_t value;
        uint8_t expected;
      } __attribute__((__packed__));

      TitleFilter(InputMemoryStream& stream);
      TitleFilter(const uint8_t* buffer, uint32_t size);
      TitleFilter(bytes const& buffer);
      TitleFilter(tid_type tid, send_mode_t mode);

      tid_type title_id() const;
      void title_id(tid_type tid);
      send_mode_t send_mode() const;
      void send_mode(send_mode_t mode);
      std::vector<title_filter_mve> mve_list() const;
      void mve_list(std::vector<title_filter_mve> const& mve_list);

      unsigned total_size() const;
      bytes to_bytes() const;
      friend std::ostream& operator<<(std::ostream& s, const TitleFilter& e);
    private:
      void parse(InputMemoryStream& stream);

      struct title_filter_header {
        uint32_t title_id;  // this one is big endian
        #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ //bitfield layout depends on endianness
        uint8_t number_mve: 4;   // number of mve in mve_list
        send_mode_t send_mode: 4;
        #else
        send_mode_t send_mode: 4;
        uint8_t number_mve: 4;
        #endif
      } __attribute__((__packed__));

      title_filter_header m_internal;
      std::vector<title_filter_mve> m_mve_list;
    };

    class KeyFilter : public Filter {
    public:
      KeyFilter(InputMemoryStream& stream);
      KeyFilter(const uint8_t* buffer, uint32_t size);
      KeyFilter(bytes const& buffer);
      KeyFilter(key_type const& k);

      key_type key() const;
      void key(key_type const& k);

      unsigned total_size() const;
      bytes to_bytes() const;
      friend std::ostream& operator<<(std::ostream& s, const KeyFilter& e);
    private:
      void parse(InputMemoryStream&);

      struct key_filter {
        uint8_t key[8];
      } __attribute__((__packed__));

      key_filter m_internal;
    };

    struct filter_list_header
    {
      #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ //bitfield layout depends on endianness
      uint8_t flags: 4;
      filter_list_marker_t marker: 4;
      #else
      filter_list_marker_t marker: 4;
      uint8_t flags: 4;
      #endif
      uint8_t length;
    } __attribute__((__packed__));

    template<class T>
    class FilterList : public ICecFormat {
      static_assert(std::is_base_of<Filter, T>::value, "T should inherit from Filter");
    public:
      static const filter_list_marker_t MARKER;
      FilterList(InputMemoryStream& stream);
      FilterList(const uint8_t* buffer, uint32_t size);
      FilterList(bytes const& buffer);
      FilterList();

      filter_list_marker_t marker() const;
      void marker(filter_list_marker_t marker);
      small_uint<4> flags() const;
      void flags(small_uint<4> flags);

      unsigned count() const;
      unsigned total_size() const;
      bytes to_bytes() const;

      template<class E>
      friend std::ostream& operator<<(std::ostream& s, const FilterList<E>& l);
    private:
      void parse(InputMemoryStream& stream);

      filter_list_header m_internal;
      std::vector<T> m_list;
    };

  private:
    void parse(InputMemoryStream& stream);
    FilterList<TitleFilter> m_title_list;
    FilterList<KeyFilter> m_key_list;
  };
}
