#pragma once

#include <cstdint>
#include <string>

namespace cutl
{

    enum class endian
    {
        little,
        big,
    };

    std::string cpp_stl_version();
    uint16_t program_bit();
    endian endian_type();
    uint16_t byteswap(uint16_t value);
    uint32_t byteswap(uint32_t value);
    uint64_t byteswap(uint64_t value);
    void byteswap(uint8_t *data, uint32_t size);

} // namespace