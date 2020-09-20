#include "cec/module_filter.hpp"

#include <tins/endianness.h>

#include <iomanip>
#include <iostream>

using namespace Tins;
using Tins::Memory::InputMemoryStream;
using Tins::Memory::OutputMemoryStream;

namespace streetpass::cec {
const char* filter_list_marker_to_string(filter_list_marker_t marker) {
  switch (marker) {
    case filter_list_marker_t::TITLE_FILTER:
      return "TITLE_FILTER";
    case filter_list_marker_t::KEY_FILTER:
      return "KEY_FILTER";
    default:
      return "invalid";
  }
}

ModuleFilter ModuleFilter::from_stream(InputMemoryStream& stream) {
  ModuleFilter filter;
  bool found_raw_bytes_list = false;
  bool found_title_list = false;
  bool found_key_list = false;

  while (stream.can_read(sizeof(filter_list_header))) {
    const filter_list_header* header =
        reinterpret_cast<const filter_list_header*>(stream.pointer());

    if (header->marker == filter_list_marker_t::RAW_BYTES_FILTER) {
      if (found_raw_bytes_list) throw "bad - already found raw bytes list";
      filter.m_raw_bytes_list = FilterList<RawBytesFilter>::from_stream(stream);
      found_raw_bytes_list = true;
    } else if (header->marker == filter_list_marker_t::TITLE_FILTER) {
      if (found_title_list) throw "bad - already found title list";
      filter.m_title_list = FilterList<TitleFilter>::from_stream(stream);
      found_title_list = true;
    } else if (header->marker == filter_list_marker_t::KEY_FILTER) {
      if (found_key_list) throw "bad - already found key list";
      filter.m_key_list = FilterList<KeyFilter>::from_stream(stream);
      found_key_list = true;
    } else {
      throw "bad marker";
    }
  }

  if (filter.m_key_list.count() != 1) throw "bad - key list count != 1";

  return filter;
}

ModuleFilter::ModuleFilter(key_type const& k) { key(k); }

ModuleFilter::FilterList<ModuleFilter::RawBytesFilter>&
ModuleFilter::raw_bytes_filters() {
  return m_raw_bytes_list;
}

ModuleFilter::FilterList<ModuleFilter::RawBytesFilter> const&
ModuleFilter::raw_bytes_filters() const {
  return m_raw_bytes_list;
}

ModuleFilter::FilterList<ModuleFilter::TitleFilter>&
ModuleFilter::title_filters() {
  return m_title_list;
}

ModuleFilter::FilterList<ModuleFilter::TitleFilter> const&
ModuleFilter::title_filters() const {
  return m_title_list;
}

key_type ModuleFilter::key() const { return m_key_list.filters().at(0).key(); }

void ModuleFilter::key(key_type const& k) {
  m_key_list.filters({KeyFilter(k)});
}

bool ModuleFilter::match(ModuleFilter const& other) const {
  return m_title_list.match(other.title_filters()) ||
         m_raw_bytes_list.match(other.raw_bytes_filters());
}

unsigned ModuleFilter::byte_size() const {
  unsigned buffer_size = m_key_list.byte_size();
  if (m_raw_bytes_list.count()) buffer_size += m_raw_bytes_list.byte_size();
  if (m_title_list.count()) buffer_size += m_title_list.byte_size();

  return buffer_size;
}

bytes ModuleFilter::to_bytes() const {
  bytes buffer(byte_size());
  OutputMemoryStream stream(buffer);
  if (m_raw_bytes_list.count()) {
    bytes rbl_bytes = m_raw_bytes_list.to_bytes();
    stream.write(rbl_bytes.data(), rbl_bytes.size());
  }
  if (m_title_list.count()) {
    bytes tl_bytes = m_title_list.to_bytes();
    stream.write(tl_bytes.data(), tl_bytes.size());
  }
  bytes cl_bytes = m_key_list.to_bytes();
  stream.write(cl_bytes.data(), cl_bytes.size());
  return buffer;
}

std::ostream& operator<<(std::ostream& s, const ModuleFilter& e) {
  s << "================================ Module Filter "
       "================================="
    << std::endl;
  s << e.m_title_list << std::endl;
  s << "-----------------------------------------------------------------------"
       "---------"
    << std::endl;
  s << e.m_key_list << std::endl;
  s << "======================================================================="
       "=========";
  return s;
}

ModuleFilter::RawBytesFilter ModuleFilter::RawBytesFilter::from_stream(
    InputMemoryStream& stream) {
  ModuleFilter::RawBytesFilter filter;
  if (!stream.can_read(sizeof(filter.m_internal))) throw "bad mve";
  stream.read(&filter.m_internal, sizeof(filter.m_internal));

  return filter;
}

ModuleFilter::RawBytesFilter::RawBytesFilter(bytes const& rb) { raw_bytes(rb); }

bytes ModuleFilter::RawBytesFilter::raw_bytes() const {
  bytes raw_bytes(m_internal.raw_bytes,
                  m_internal.raw_bytes + sizeof(m_internal.raw_bytes));
  return raw_bytes;
}

void ModuleFilter::RawBytesFilter::raw_bytes(bytes const& rb) {
  if (rb.size() > sizeof(m_internal.raw_bytes))
    throw std::length_error("raw bytes filter length cannot exceed 16 bytes");
  std::memcpy(m_internal.raw_bytes, rb.data(), rb.size());
}

unsigned ModuleFilter::RawBytesFilter::byte_size() const {
  return sizeof(m_internal);
}

bool ModuleFilter::RawBytesFilter::match(
    ModuleFilter::RawBytesFilter const& other) const {
  return (m_internal.cmp_length == other.m_internal.cmp_length) &&
         std::equal(m_internal.raw_bytes,
                    m_internal.raw_bytes + m_internal.cmp_length,
                    other.m_internal.raw_bytes);
}

bytes ModuleFilter::RawBytesFilter::to_bytes() const {
  bytes buffer(sizeof(m_internal));
  OutputMemoryStream stream(buffer);
  stream.write(m_internal);

  return buffer;
}

std::ostream& operator<<(std::ostream& s,
                         const ModuleFilter::RawBytesFilter& f) {
  std::stringstream ss;
  bytes raw_bytes = f.raw_bytes();
  ss << std::hex;
  ss << "RawBytes: ";
  ss << "size=" << (int)raw_bytes.size() << ", ";
  ss << "bytes=";
  for (uint8_t b : raw_bytes) ss << " " << (int)b;

  s << ss.str();
  return s;
}

ModuleFilter::TitleFilter::MVE ModuleFilter::TitleFilter::MVE::from_stream(
    InputMemoryStream& stream) {
  MVE mve;
  if (!stream.can_read(sizeof(mve.m_internal))) throw "bad mve";
  stream.read(&mve.m_internal, sizeof(mve.m_internal));

  return mve;
}

ModuleFilter::TitleFilter::MVE::MVE(uint8_t m, uint8_t v, uint8_t e) {
  mask(m);
  value(v);
  expectation(e);
}

uint8_t ModuleFilter::TitleFilter::MVE::mask() const { return m_internal.mask; }

void ModuleFilter::TitleFilter::MVE::mask(uint8_t m) { m_internal.mask = m; }

uint8_t ModuleFilter::TitleFilter::MVE::value() const {
  return m_internal.value;
}

void ModuleFilter::TitleFilter::MVE::value(uint8_t v) { m_internal.value = v; }

uint8_t ModuleFilter::TitleFilter::MVE::expectation() const {
  return m_internal.expectation;
}

void ModuleFilter::TitleFilter::MVE::expectation(uint8_t e) {
  m_internal.expectation = e;
}

bool ModuleFilter::TitleFilter::MVE::match(
    ModuleFilter::TitleFilter::MVE const& other) const {
  return ((this->mask() & this->expectation()) ==
          (this->mask() & other.value())) &&
         ((other.mask() & other.expectation()) ==
          (other.mask() & this->value()));
}

bytes ModuleFilter::TitleFilter::MVE::to_bytes() const {
  bytes buffer(sizeof(m_internal));
  OutputMemoryStream stream(buffer);
  stream.write(m_internal);

  return buffer;
}

std::ostream& operator<<(std::ostream& s,
                         const ModuleFilter::TitleFilter::MVE& e) {
  std::stringstream ss;
  ss << std::hex;
  ss << "MVE[mask=" << (int)e.mask();
  ss << ", value=" << (int)e.value();
  ss << ", expectation=" << (int)e.expectation();
  ss << "]";

  s << ss.str();
  return s;
}

ModuleFilter::TitleFilter ModuleFilter::TitleFilter::from_stream(
    InputMemoryStream& stream) {
  TitleFilter filter;
  if (!stream.can_read(sizeof(filter.m_internal))) throw "bad title filter";

  stream.read(&filter.m_internal, sizeof(filter.m_internal));
  if (filter.m_internal.send_mode > SendMode::SEND_RECV)
    throw "bad send mode";

  uint8_t mve_count = filter.m_internal.number_mve;
  while (mve_count) {
    filter.m_mve_list.push_back(MVE::from_stream(stream));
    mve_count--;
  }

  return filter;
}

ModuleFilter::TitleFilter::TitleFilter(tid_type tid, SendMode mode,
                                       std::vector<MVE> list) {
  title_id(tid);
  send_mode(mode);
  mve_list(list);
}

tid_type ModuleFilter::TitleFilter::title_id() const {
  return Endian::be_to_host(m_internal.title_id);
}

void ModuleFilter::TitleFilter::title_id(tid_type tid) {
  m_internal.title_id = Endian::host_to_be(tid);
}

SendMode ModuleFilter::TitleFilter::send_mode() const {
  return m_internal.send_mode;
}

void ModuleFilter::TitleFilter::send_mode(SendMode mode) {
  m_internal.send_mode = mode;
}

std::vector<ModuleFilter::TitleFilter::MVE> const&
ModuleFilter::TitleFilter::mve_list() const {
  return m_mve_list;
}

void ModuleFilter::TitleFilter::mve_list(
    std::vector<ModuleFilter::TitleFilter::MVE> const& mve_list) {
  if (mve_list.size() > 0xF)
    throw std::length_error("MVE list size cannot exceed 15");

  m_mve_list = mve_list;
  m_internal.number_mve = mve_list.size();
}

unsigned ModuleFilter::TitleFilter::byte_size() const {
  return sizeof(m_internal) +
         m_mve_list.size() * ModuleFilter::TitleFilter::MVE::fixed_byte_size();
}

bool ModuleFilter::TitleFilter::match(
    ModuleFilter::TitleFilter const& other) const {
  if (this->title_id() != other.title_id()) return false;
  if (!this->send_mode().match(other.send_mode())) return false;

  std::vector<MVE> const& own_list = this->mve_list();
  std::vector<MVE> const& other_list = other.mve_list();
  if (own_list.size() != other_list.size()) return false;
  bool match = true;
  for (unsigned i = 0; match && i < own_list.size(); i++)
    match &= own_list[i].match(other_list[i]);

  return match;
}

bytes ModuleFilter::TitleFilter::to_bytes() const {
  bytes buffer(sizeof(m_internal) +
               m_mve_list.size() *
                   ModuleFilter::TitleFilter::MVE::fixed_byte_size());
  OutputMemoryStream stream(buffer);
  stream.write(m_internal);

  for (ModuleFilter::TitleFilter::MVE const& mve : m_mve_list) {
    bytes mve_bytes = mve.to_bytes();
    stream.write(mve_bytes.data(), mve_bytes.size());
  }

  return buffer;
}

std::ostream& operator<<(std::ostream& s, const ModuleFilter::TitleFilter& e) {
  std::stringstream ss;
  ss << "Title: ";
  ss << "id=" << std::hex << std::setfill('0') << std::setw(8) << e.title_id()
     << ", send_mode=" << std::setw(2) << static_cast<unsigned>(e.send_mode())
     << "(" << e.send_mode().to_string() << ")";
  if (e.m_mve_list.size()) {
    ss << ", mve_list=";
    // TODO: better printer for MVE list
    for (ModuleFilter::TitleFilter::MVE const& mve : e.m_mve_list)
      ss << " " << mve;
  }

  s << ss.str();
  return s;
}

ModuleFilter::KeyFilter ModuleFilter::KeyFilter::from_stream(
    InputMemoryStream& stream) {
  KeyFilter filter;
  if (!stream.can_read(sizeof(filter.m_internal))) throw "bad key filter";
  stream.read(&filter.m_internal, sizeof(filter.m_internal));

  return filter;
}

ModuleFilter::KeyFilter::KeyFilter(key_type const& k) { key(k); }

key_type ModuleFilter::KeyFilter::key() const {
  key_type key;
  std::copy(std::begin(m_internal.key), std::end(m_internal.key), key.begin());
  return key;
}

void ModuleFilter::KeyFilter::key(key_type const& k) {
  std::memcpy(m_internal.key, k.data(), sizeof(m_internal.key));
}

unsigned ModuleFilter::KeyFilter::byte_size() const {
  return sizeof(m_internal);
}

bool ModuleFilter::KeyFilter::match(ModuleFilter::KeyFilter const&) const {
  return true;
}

bytes ModuleFilter::KeyFilter::to_bytes() const {
  bytes buffer(sizeof(m_internal));
  OutputMemoryStream stream(buffer);
  stream.write(m_internal);
  return buffer;
}

std::ostream& operator<<(std::ostream& s, const ModuleFilter::KeyFilter& e) {
  std::stringstream ss;
  ss << "key=";
  ss << std::hex << std::setfill('0') << std::setw(2);
  for (unsigned b : e.m_internal.key) ss << std::setw(2) << b;

  s << ss.str();
  return s;
}

template <class T>
ModuleFilter::FilterList<T> ModuleFilter::FilterList<T>::from_stream(
    InputMemoryStream& stream) {
  ModuleFilter::FilterList<T> filter_list;
  if (!stream.can_read(sizeof(filter_list.m_internal))) throw "bad list header";
  stream.read(&filter_list.m_internal, sizeof(filter_list.m_internal));
  if (filter_list.m_internal.marker != MARKER) throw "type mismatch";
  if (!stream.can_read(filter_list.m_internal.length)) throw "bad length";

  InputMemoryStream elt_steam(stream.pointer(), filter_list.m_internal.length);
  while (elt_steam.size() > 0) {
    T t = T::from_stream(elt_steam);
    filter_list.m_list.push_back(t);
    stream.skip(t.byte_size());
  }

  return filter_list;
}

template <class T>
ModuleFilter::FilterList<T>::FilterList() : m_internal{} {
  marker(MARKER);
}

template <class T>
filter_list_marker_t ModuleFilter::FilterList<T>::marker() const {
  return m_internal.marker;
}

template <class T>
void ModuleFilter::FilterList<T>::marker(filter_list_marker_t marker) {
  m_internal.marker = marker;
}

template <class T>
small_uint<4> ModuleFilter::FilterList<T>::flags() const {
  return m_internal.flags;
}

template <class T>
void ModuleFilter::FilterList<T>::flags(small_uint<4> flags) {
  m_internal.flags = flags;
}

template <class T>
unsigned ModuleFilter::FilterList<T>::byte_size() const {
  unsigned size = sizeof(m_internal);
  for (T const& e : m_list) size += e.byte_size();
  return size;
}

template <class T>
std::vector<T> const& ModuleFilter::FilterList<T>::filters() const {
  return m_list;
}

template <class T>
void ModuleFilter::FilterList<T>::filters(std::vector<T> const& filters) {
  uint8_t byte_size = 0;
  for (T filter : filters) {
    unsigned filter_size = filter.byte_size();
    if (filter_size + byte_size > byte_size)
      throw std::length_error("Byte size of filter list cannot exceed 0xFF");
    byte_size += filter.byte_size();
  }
  m_list = filters;
}

template <class T>
bool ModuleFilter::FilterList<T>::match(
    ModuleFilter::FilterList<T> const& other) const {
  for (unsigned i = 0; this->count(); i++) {
    for (unsigned j = i; other.count(); j++)
      if (this->filters()[i].match(other.filters()[j])) return true;
  }

  return false;
}

template <class T>
unsigned ModuleFilter::FilterList<T>::count() const {
  return m_list.size();
}

template <class T>
bytes ModuleFilter::FilterList<T>::to_bytes() const {
  bytes buffer(sizeof(m_internal) + m_internal.length);
  OutputMemoryStream stream(buffer);
  stream.write(m_internal);
  for (T const& e : m_list) {
    bytes elt_bytes = e.to_bytes();
    stream.write(elt_bytes.data(), elt_bytes.size());
  }
  return buffer;
}

template <typename E>
std::ostream& operator<<(std::ostream& s,
                         const ModuleFilter::FilterList<E>& l) {
  std::stringstream ss;
  ss << "FilterList: ";
  ss << std::hex << std::setfill('0');
  ss << "marker=" << std::setw(2) << static_cast<unsigned>(l.marker()) << "("
     << filter_list_marker_to_string(l.marker()) << "), ";
  ss << "flags=" << std::setw(2) << static_cast<unsigned>(l.flags()) << ", ";
  ss << "length=" << std::setw(2) << static_cast<unsigned>(l.m_internal.length)
     << std::endl;
  ss << "Content:";
  for (auto const& e : l.m_list) ss << std::endl << e;
  s << ss.str();
  return s;
}

template <>
const filter_list_marker_t
    ModuleFilter::FilterList<ModuleFilter::RawBytesFilter>::MARKER =
        filter_list_marker_t::RAW_BYTES_FILTER;
template <>
const filter_list_marker_t
    ModuleFilter::FilterList<ModuleFilter::TitleFilter>::MARKER =
        filter_list_marker_t::TITLE_FILTER;
template <>
const filter_list_marker_t
    ModuleFilter::FilterList<ModuleFilter::KeyFilter>::MARKER =
        filter_list_marker_t::KEY_FILTER;

template class ModuleFilter::FilterList<ModuleFilter::RawBytesFilter>;
template class ModuleFilter::FilterList<ModuleFilter::TitleFilter>;
template class ModuleFilter::FilterList<ModuleFilter::KeyFilter>;
}  // namespace streetpass::cec
