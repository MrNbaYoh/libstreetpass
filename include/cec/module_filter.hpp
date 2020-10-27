#pragma once

#include <tins/memory_helpers.h>
#include <tins/small_uint.h>

#include <array>
#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "cec/cec.hpp"
#include "cec/endian_types.hpp"
#include "cec/send_mode.hpp"

using namespace streetpass::cec::endian_types;
using Tins::small_uint;
using Tins::Memory::InputMemoryStream;

namespace streetpass::cec {
class ModuleFilter : public ICecFormat {
 public:
  class FilterListMarker {
   public:
    enum filter_list_marker : uint8_t {
      RAW_BYTES_FILTER = 0x0,
      TITLE_FILTER = 0x1,
      KEY_FILTER = 0xF
    };

    constexpr FilterListMarker(filter_list_marker marker) : m_value(marker) {}

    operator filter_list_marker() const { return m_value; }
    explicit operator bool() = delete;
    explicit operator std::string() const;

    friend std::ostream& operator<<(std::ostream& s, const FilterListMarker& m);

   private:
    filter_list_marker m_value;
  };

  template <typename T>
  class Filter : public ICecFormat {
   public:
    virtual bool match(T const& other) const = 0;
  };

  class RawBytesFilter : public Filter<RawBytesFilter> {
   public:
    static RawBytesFilter from_stream(InputMemoryStream& stream);

    RawBytesFilter(bytes const& raw_bytes);

    bytes raw_bytes() const;
    void raw_bytes(bytes const& raw_bytes);

    unsigned byte_size() const;
    bool match(RawBytesFilter const& other) const;
    explicit operator bytes() const;
    friend std::ostream& operator<<(std::ostream& s, const RawBytesFilter& f);

   private:
    RawBytesFilter() = default;

    struct raw_filter {
      u8 cmp_length;
      u8 raw_bytes[16];
    };

    raw_filter m_internal;
  };

  class TitleFilter : public Filter<TitleFilter> {
   public:
    class MVE : public ICecFormat {
     public:
      static MVE from_stream(InputMemoryStream& stream);

      MVE(uint8_t mask, uint8_t value, uint8_t expectation);

      uint8_t mask() const;
      void mask(uint8_t m);
      uint8_t value() const;
      void value(uint8_t v);
      uint8_t expectation() const;
      void expectation(uint8_t e);

      bool match(MVE const& other) const;

      explicit operator bytes() const;
      friend std::ostream& operator<<(std::ostream& s, const ModuleFilter& l);

      constexpr unsigned byte_size() const { return sizeof(title_filter_mve); }

      constexpr static unsigned fixed_byte_size() {
        return sizeof(title_filter_mve);
      }

     private:
      MVE() = default;

      struct title_filter_mve {
        u8 mask;
        u8 value;
        u8 expectation;
      } __attribute__((__packed__));

      title_filter_mve m_internal;
    };

    static TitleFilter from_stream(InputMemoryStream& stream);

    TitleFilter(tid_type tid, SendMode mode, std::vector<MVE> mve_list);

    tid_type title_id() const;
    void title_id(tid_type tid);
    SendMode send_mode() const;
    void send_mode(SendMode mode);
    std::vector<MVE> const& mve_list() const;
    void mve_list(std::vector<MVE> const& mve_list);

    unsigned byte_size() const;
    bool match(TitleFilter const& other) const;
    explicit operator bytes() const;
    friend std::ostream& operator<<(std::ostream& s, const TitleFilter& e);

   private:
    TitleFilter() = default;

    struct title_filter_header_le {
      u32be title_id;  // this one is big endian
      // bitfield layout depends on endianness
      u8 number_mve : 4;  // number of mve in mve_list
      SendMode::send_mode send_mode : 4;
    } __attribute__((__packed__));

    struct title_filter_header_be {
      u32be title_id;  // this one is big endian
      // bitfield layout depends on endianness
      SendMode::send_mode send_mode : 4;
      u8 number_mve : 4;  // number of mve in mve_list
    } __attribute__((__packed__));

    using title_filter_header =
        std::conditional<__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__,
                         title_filter_header_le, title_filter_header_be>::type;

    title_filter_header m_internal;
    std::vector<MVE> m_mve_list;
  };

  class KeyFilter : public Filter<KeyFilter> {
   public:
    static KeyFilter from_stream(InputMemoryStream& stream);

    KeyFilter(key_type const& k);

    key_type key() const;
    void key(key_type const& k);

    unsigned byte_size() const;
    bool match(KeyFilter const& other) const;
    explicit operator bytes() const;
    friend std::ostream& operator<<(std::ostream& s, const KeyFilter& e);

   private:
    KeyFilter() = default;

    struct key_filter {
      u8 key[8];
    } __attribute__((__packed__));

    key_filter m_internal;
  };

  struct filter_list_header_le {
    // bitfield layout depends on endianness
    u8 flags : 4;
    FilterListMarker::filter_list_marker marker : 4;
    u8 length;
  } __attribute__((__packed__));

  struct filter_list_header_be {
    // bitfield layout depends on endianness
    FilterListMarker::filter_list_marker marker : 4;
    u8 flags : 4;
    u8 length;
  } __attribute__((__packed__));

  using filter_list_header =
      std::conditional<__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__,
                       filter_list_header_le, filter_list_header_be>::type;

  template <class T>
  class FilterList : public ICecFormat {
    static_assert(std::is_base_of<Filter<T>, T>::value,
                  "T should inherit from Filter");

   public:
    static const FilterListMarker MARKER;

    static FilterList<T> from_stream(InputMemoryStream& stream);

    FilterList();

    small_uint<4> flags() const;
    void flags(small_uint<4> flags);
    std::vector<T> const& filters() const;
    void filters(std::vector<T> const& filters);

    unsigned count() const;
    unsigned byte_size() const;
    bool match(FilterList<T> const& other) const;
    explicit operator bytes() const;

    template <class E>
    friend std::ostream& operator<<(std::ostream& s, const FilterList<E>& l);

   private:
    filter_list_header m_internal;
    std::vector<T> m_list;
  };

  static ModuleFilter from_stream(InputMemoryStream& stream);

  ModuleFilter(key_type const& k);

  FilterList<RawBytesFilter>& raw_bytes_filters();
  FilterList<RawBytesFilter> const& raw_bytes_filters() const;
  FilterList<TitleFilter>& title_filters();
  FilterList<TitleFilter> const& title_filters() const;
  key_type key() const;
  void key(key_type const& k);

  bool match(ModuleFilter const& other) const;
  unsigned byte_size() const;
  explicit operator bytes() const;
  friend std::ostream& operator<<(std::ostream& s, const ModuleFilter& l);

 private:
  ModuleFilter() = default;

  FilterList<RawBytesFilter> m_raw_bytes_list;
  FilterList<TitleFilter> m_title_list;
  FilterList<KeyFilter> m_key_list;
};
}  // namespace streetpass::cec
