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
  enum class list_marker_t : uint8_t {
    TITLE_LIST = 0x1,
    CONSOLE_ID = 0xF
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

    class ListElement : public ICecFormat {
    public:
      virtual unsigned total_size() const = 0;
    };

    class TitleElement : public ListElement {
    public:
      TitleElement(InputMemoryStream& stream);
      TitleElement(const uint8_t* buffer, uint32_t size);
      TitleElement(bytes const& buffer);
      TitleElement(tid_type tid, send_mode_t mode, bytes const& data);

      tid_type title_id() const;
      void title_id(tid_type tid);
      send_mode_t send_mode() const;
      void send_mode(send_mode_t mode);
      bytes extra_data() const;
      void extra_data(bytes const& data);

      unsigned total_size() const;
      bytes to_bytes() const;
      friend std::ostream& operator<<(std::ostream& s, const TitleElement& e);
    private:
      void parse(InputMemoryStream& stream);

      struct title_element {
        uint32_t title_id;  // this one is big endian
        #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ //bitfield layout depends on endianness
        uint8_t extra_triplet: 4;   // number of extra triplet of bytes
        send_mode_t send_mode: 4;
        #else
        send_mode_t send_mode: 4;
        uint8_t extra_triplet: 4;
        #endif
      } __attribute__((__packed__));

      title_element m_internal;
      bytes m_extra_data;
    };

    class ConsoleIdElement : public ListElement {
    public:
      ConsoleIdElement(InputMemoryStream& stream);
      ConsoleIdElement(const uint8_t* buffer, uint32_t size);
      ConsoleIdElement(bytes const& buffer);
      ConsoleIdElement(cid_type const& cid);

      cid_type console_id() const;
      void console_id(cid_type const& cid);

      unsigned total_size() const;
      bytes to_bytes() const;
      friend std::ostream& operator<<(std::ostream& s, const ConsoleIdElement& e);
    private:
      void parse(InputMemoryStream&);

      struct cid_element {
        uint8_t console_id[8];
      } __attribute__((__packed__));

      cid_element m_internal;
    };

    struct list_header
    {
      #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ //bitfield layout depends on endianness
      uint8_t flags: 4;
      list_marker_t marker: 4;
      #else
      list_marker_t marker: 4;
      uint8_t flags: 4;
      #endif
      uint8_t length;
    } __attribute__((__packed__));

    template<class T>
    class List : public ICecFormat {
      static_assert(std::is_base_of<ListElement, T>::value, "T should inherit from ListElement");
    public:
      static const list_marker_t MARKER;
      List(InputMemoryStream& stream);
      List(const uint8_t* buffer, uint32_t size);
      List(bytes const& buffer);
      List();

      list_marker_t marker() const;
      void marker(list_marker_t marker);
      small_uint<4> flags() const;
      void flags(small_uint<4> flags);

      unsigned count() const;
      unsigned total_size() const;
      bytes to_bytes() const;

      template<class E>
      friend std::ostream& operator<<(std::ostream& s, const List<E>& l);
    private:
      void parse(InputMemoryStream& stream);

      list_header m_internal;
      std::vector<T> m_list;
    };

  private:
    void parse(InputMemoryStream& stream);
    List<TitleElement> m_title_list;
    List<ConsoleIdElement> m_cid_list;
  };
}
