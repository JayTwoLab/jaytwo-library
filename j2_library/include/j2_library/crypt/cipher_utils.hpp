#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include "j2_library/export.hpp"

namespace j2 {
    namespace crypt {

        class J2LIB_API cipher_utils {
        public:
            static std::string to_hex(const std::vector<uint8_t>& data);
            static std::vector<uint8_t> from_hex(const std::string& hex);
            static std::vector<uint8_t> add_padding(const std::string& input, size_t blockSize = 16);
            static std::string remove_padding(const std::vector<uint8_t>& input);
        };

    } // namespace crypt
} // namespace j2
