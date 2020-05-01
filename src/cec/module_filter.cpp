#include "cec/module_filter.hpp"

#include <tins/endianness.h>
#include <iostream>
#include <iomanip>
using namespace Tins;
using Tins::Memory::InputMemoryStream;
using Tins::Memory::OutputMemoryStream;

namespace streetpass::cec {
  ModuleFilter::ModuleFilter(InputMemoryStream& stream) {
    parse(stream);
  }

  ModuleFilter::ModuleFilter(const uint8_t* buffer, uint32_t size) {
    InputMemoryStream stream(buffer, size);
    parse(stream);
  }

  ModuleFilter::ModuleFilter(bytes const& buffer) :
    ModuleFilter(buffer.data(), buffer.size()) {}

  bytes ModuleFilter::to_bytes() const {
    bytes buffer(m_title_list.total_size() + m_cid_list.total_size());
    OutputMemoryStream stream(buffer);
    bytes tl_bytes = m_title_list.to_bytes();
    stream.write(tl_bytes.data(), tl_bytes.size());
    bytes cl_bytes = m_cid_list.to_bytes();
    stream.write(cl_bytes.data(), cl_bytes.size());
    return buffer;
  }

  void ModuleFilter::parse(InputMemoryStream& stream) {
    bool found_title_list = false;
    bool found_cid_list = false;

    while(stream.can_read(sizeof(list_header))) {
      const list_header* header =
        reinterpret_cast<const list_header*>(stream.pointer());
      if(header->marker == list_marker_t::TITLE_LIST) {
        if(found_title_list)
          throw "bad - already found title list";
        m_title_list = List<TitleElement>(stream);
      } else if(header->marker == list_marker_t::CONSOLE_ID) {
        if(found_cid_list)
          throw "bad - already found cid list";
        m_cid_list = List<ConsoleIdElement>(stream);
        if(m_cid_list.count() != 1)
          throw "bad - cid list count != 1";
      } else {
        std::cerr << std::hex << (int)header->flags << std::endl;
        throw "bad marker";
      }
    }
  }

  std::ostream& operator<<(std::ostream& s, const ModuleFilter& e) {
    s << "================================ Module Filter =================================" << std::endl;
    s << e.m_title_list << std::endl;
    s << "--------------------------------------------------------------------------------" << std::endl;
    s << e.m_cid_list << std::endl;
    s << "================================================================================";
    return s;
  }

  ModuleFilter::TitleElement::TitleElement(InputMemoryStream& stream) {
    parse(stream);
  }

  ModuleFilter::TitleElement::TitleElement(const uint8_t* buffer, uint32_t size) {
    InputMemoryStream stream(buffer, size);
    parse(stream);
  }

  ModuleFilter::TitleElement::TitleElement(bytes const& buffer) :
    TitleElement(buffer.data(), buffer.size()) {}

  ModuleFilter::TitleElement::TitleElement(tid_type tid, send_mode_t mode, bytes const& data) {
    title_id(tid);
    send_mode(mode);
    extra_data(data);
  }

  void ModuleFilter::TitleElement::parse(InputMemoryStream& stream) {
    if(!stream.can_read(sizeof(m_internal)))
      throw "bad title element";
    stream.read(&m_internal, sizeof(m_internal));
    uint8_t extra_size = m_internal.extra_triplet*3;
    if(extra_size)
    {
      if(!stream.can_read(extra_size))
        throw "bad read extra data";
      stream.read(m_extra_data, extra_size);
    }
  }

  tid_type ModuleFilter::TitleElement::title_id() const {
    return Endian::be_to_host(m_internal.title_id);
  }

  void ModuleFilter::TitleElement::title_id(tid_type tid) {
    m_internal.title_id = Endian::host_to_be(tid);
  }

  send_mode_t ModuleFilter::TitleElement::send_mode() const {
    return m_internal.send_mode;
  }

  void ModuleFilter::TitleElement::send_mode(send_mode_t mode) {
    m_internal.send_mode = mode;
  }

  bytes ModuleFilter::TitleElement::extra_data() const {
    return m_extra_data;
  }

  void ModuleFilter::TitleElement::extra_data(bytes const& data) {
    unsigned aligned_size = ((data.size() + 2) / 3) * 3;
    unsigned triplet_count = aligned_size/3;
    if(triplet_count > 0xF)
      throw std::length_error("Extra data length cannot exceed 0x2D bytes");

    m_extra_data = data;
    if(aligned_size - data.size())
      m_extra_data.insert(m_extra_data.end(), aligned_size - data.size(), 0);

    m_internal.extra_triplet = m_extra_data.size()/3;
  }

  unsigned ModuleFilter::TitleElement::total_size() const {
    return sizeof(m_internal) + m_extra_data.size();
  }

  bytes ModuleFilter::TitleElement::to_bytes() const {
    bytes buffer(sizeof(m_internal) + m_extra_data.size());
    OutputMemoryStream stream(buffer);
    stream.write(m_internal);
    stream.write(m_extra_data.data(), m_extra_data.size());
    return buffer;
  }

  std::ostream& operator<<(std::ostream& s, const ModuleFilter::TitleElement& e) {
    std::stringstream ss;
    ss << "Title: ";
    ss <<"id=" << std::hex << std::setfill('0') << std::setw(8) << e.title_id()
      << ", send_mode=" << std::setw(2) << static_cast<unsigned>(e.send_mode());
    if(e.m_extra_data.size()) {
      ss << ", extra_data=";
      for(unsigned b: e.m_extra_data)
        ss << std::setw(2) << b;
    }

    s << ss.str();
    return s;
  }

  ModuleFilter::ConsoleIdElement::ConsoleIdElement(InputMemoryStream& stream) {
    parse(stream);
  }

  ModuleFilter::ConsoleIdElement::ConsoleIdElement(const uint8_t* buffer, uint32_t size) {
    InputMemoryStream stream(buffer, size);
    parse(stream);
  }

  ModuleFilter::ConsoleIdElement::ConsoleIdElement(bytes const& buffer) :
    ConsoleIdElement(buffer.data(), buffer.size()) {}

  ModuleFilter::ConsoleIdElement::ConsoleIdElement(cid_type const& cid) {
    console_id(cid);
  }

  void ModuleFilter::ConsoleIdElement::parse(InputMemoryStream& stream) {
    if(!stream.can_read(sizeof(m_internal)))
      throw "bad console id element";
    stream.read(&m_internal, sizeof(m_internal));
  }

  cid_type ModuleFilter::ConsoleIdElement::console_id() const {
    cid_type cid;
    std::copy(std::begin(m_internal.console_id), std::end(m_internal.console_id), cid.begin());
    return cid;
  }

  void ModuleFilter::ConsoleIdElement::console_id(cid_type const& cid) {
    std::memcpy(m_internal.console_id, cid.data(), sizeof(m_internal.console_id));
  }

  unsigned ModuleFilter::ConsoleIdElement::total_size() const {
    return sizeof(m_internal);
  }

  bytes ModuleFilter::ConsoleIdElement::to_bytes() const {
    bytes buffer(sizeof(m_internal));
    OutputMemoryStream stream(buffer);
    stream.write(m_internal);
    return buffer;
  }

  std::ostream& operator<<(std::ostream& s, const ModuleFilter::ConsoleIdElement& e) {
    std::stringstream ss;
    ss << "cid=";
    ss << std::hex << std::setfill('0') << std::setw(2);
    for(unsigned b: e.m_internal.console_id)
      ss << std::setw(2) << b;

    s << ss.str();
    return s;
  }

  template<class T>
  ModuleFilter::List<T>::List(InputMemoryStream& stream) {
    parse(stream);
  }

  template<class T>
  ModuleFilter::List<T>::List(const uint8_t* buffer, uint32_t size) {
    InputMemoryStream stream(buffer, size);
    parse(stream);
  }

  template<class T>
  ModuleFilter::List<T>::List() : m_internal{} {
    marker(MARKER);
  }

  template<class T>
  ModuleFilter::List<T>::List(bytes const& buffer) :
    List<T>(buffer.data(), buffer.size()) {}

  template<class T>
  void ModuleFilter::List<T>::parse(InputMemoryStream& stream) {
    if(!stream.can_read(sizeof(m_internal)))
      throw "bad list header";
    stream.read(&m_internal, sizeof(m_internal));
    if(m_internal.marker != MARKER)
      throw "type mismatch";
    if(!stream.can_read(m_internal.length))
      throw "bad length";

    InputMemoryStream elt_steam(stream.pointer(), m_internal.length);
    while(elt_steam.size() > 0) {
      T t(elt_steam);
      m_list.push_back(t);
      stream.skip(t.total_size());
    }
  }

  template<class T>
  list_marker_t ModuleFilter::List<T>::marker() const {
    return m_internal.marker;
  }

  template<class T>
  void ModuleFilter::List<T>::marker(list_marker_t marker) {
    m_internal.marker = marker;
  }

  template<class T>
  small_uint<4> ModuleFilter::List<T>::flags() const {
    return m_internal.flags;
  }

  template<class T>
  void ModuleFilter::List<T>::flags(small_uint<4> flags) {
    m_internal.flags = flags;
  }

  template<class T>
  unsigned ModuleFilter::List<T>::total_size() const {
    unsigned size = sizeof(m_internal);
    for(T const& e: m_list)
      size += e.total_size();
    return size;
  }

  template<class T>
  unsigned ModuleFilter::List<T>::count() const {
    return m_list.size();
  }

  template<class T>
  bytes ModuleFilter::List<T>::to_bytes() const {
    bytes buffer(sizeof(m_internal) + m_internal.length);
    OutputMemoryStream stream(buffer);
    stream.write(m_internal);
    for(T const& e: m_list) {
      bytes elt_bytes = e.to_bytes();
      stream.write(elt_bytes.data(), elt_bytes.size());
    }
    return buffer;
  }

  template<typename E>
  std::ostream& operator<<(std::ostream& s,
    const ModuleFilter::List<E>& l)
  {
    std::stringstream ss;
    ss << "List: ";
    ss << std::hex << std::setfill('0');
    ss << "marker=" << std::setw(2) << static_cast<unsigned>(l.marker()) << ", ";
    ss << "flags=" << std::setw(2) << static_cast<unsigned>(l.flags()) << ", ";
    ss << "length=" << std::setw(2) << static_cast<unsigned>(l.m_internal.length) << std::endl;
    ss << "Content:";
    for(auto const& e: l.m_list)
      ss << std::endl << e;
    s << ss.str();
    return s;
  }

  template<>
  const list_marker_t ModuleFilter::List<ModuleFilter::TitleElement>::MARKER =
    list_marker_t::TITLE_LIST;
  template<>
  const list_marker_t ModuleFilter::List<ModuleFilter::ConsoleIdElement>::MARKER =
    list_marker_t::CONSOLE_ID;

  template class ModuleFilter::List<ModuleFilter::TitleElement>;
  template class ModuleFilter::List<ModuleFilter::ConsoleIdElement>;
}
