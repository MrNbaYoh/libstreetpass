#include "crypto/crypto.hpp"

#include <cryptopp/secblock.h>
#include <cryptopp/filters.h>
#include <cryptopp/files.h>
#include <cryptopp/hex.h>
#include <cryptopp/modes.h>
#include <cryptopp/aes.h>
#include <cryptopp/hmac.h>
#include <cryptopp/sha.h>

namespace crypto {
  using namespace CryptoPP;

  SecByteBlock normal_key(0x00, AES::BLOCKSIZE);
  SecByteBlock cecd_key(0x00, 17);

  void load_normal_key(std::string const& filepath) {
    FileSource fs(filepath.c_str(), true,
      new ArraySink(normal_key.begin(), normal_key.size()));
  }

  void load_normal_key(std::array<std::uint8_t, 16> const& key) {
    ArraySource as(key.data(), key.size(), true,
      new ArraySink(normal_key.begin(), normal_key.size()));
  }

  void load_cecd_key(std::string const& filepath) {
    FileSource fs(filepath.c_str(), true,
      new ArraySink(cecd_key.begin(), cecd_key.size()));
  }

  void load_cecd_key(std::array<std::uint8_t, 17> const& key) {
    ArraySource as(key.data(), key.size(), true,
      new ArraySink(cecd_key.begin(), cecd_key.size()));
  }

  std::array<std::uint8_t, 16> streetpass_ccmp_key(
    std::array<std::uint8_t, 8> const& master_cid,
    std::array<std::uint8_t, 6> const& master_mac,
    std::array<std::uint8_t, 8> const& client_cid,
    std::array<std::uint8_t, 6> const& client_mac)
  {
    std::vector<std::uint8_t> hash_block;
    hash_block.insert(hash_block.end(), master_cid.begin(), master_cid.end());
    hash_block.insert(hash_block.end(), client_cid.begin(), client_cid.end());
    hash_block.insert(hash_block.end(), master_mac.begin(), master_mac.end());
    hash_block.insert(hash_block.end(), client_mac.begin(), client_mac.end());

    std::array<std::uint8_t, AES::BLOCKSIZE> ctr = {};
    HMAC<SHA1> hmac(cecd_key, cecd_key.size());
    ArraySource as1(hash_block.data(), hash_block.size(), true,
      new HashFilter(hmac, new ArraySink(ctr.begin(), ctr.size())));

    std::array<std::uint8_t, AES::BLOCKSIZE> null_block = {};
    std::array<std::uint8_t, AES::BLOCKSIZE> ccmp_key = {};
    CTR_Mode< AES >::Encryption e;
    e.SetKeyWithIV(normal_key, normal_key.size(), ctr.data());

    ArraySource as2(null_block.data(), null_block.size(), true,
      new StreamTransformationFilter(e,
        new ArraySink(ccmp_key.begin(), ccmp_key.size())));
    return ccmp_key;
  }
}
