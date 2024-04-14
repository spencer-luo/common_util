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

    enum class platform
    {
        windows,
        macos,
        linux,
        unix,
        unknown,
    };

    platform platform_type();
    std::string platform_name();
    std::string cpp_stl_version();
    uint16_t program_bit();

    endian endian_type();
    uint16_t byteswap(uint16_t value);
    uint32_t byteswap(uint32_t value);
    uint64_t byteswap(uint64_t value);
    void byteswap(uint8_t *data, uint32_t size);

    bool system(const std::string &cmd);
    bool callcmd(const std::string &cmd, std::string &result);
    std::string getenv(const std::string &name, const std::string &default_value);

} // namespace