#pragma once

#include <vector>
#include <cstdint>
#include <array>
#include <tins/memory_helpers.h>

using Tins::Memory::InputMemoryStream;

namespace streetpass::cec {
  using cid_type = std::array<uint8_t, 8>;
  using tid_type = uint32_t;
  using bytes = std::vector<uint8_t>;

  class ICecFormat {
  private:
    virtual void parse(InputMemoryStream&) = 0;
  public:
    virtual bytes to_bytes() const = 0;
  };
}
