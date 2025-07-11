/**
 * @copyright Copyright (c) 2024, Spencer.Luo. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations.
 *
 * @file sysutil.h
 * @brief Common system utilities.
 * @author spencer.luo
 * @date 2024-05-13
 */

#pragma once

#include <cstdint>
#include <string>

namespace cutl
{

    /**
     * @brief Endianness type.
     *
     */
    enum class endian
    {
        /** little endian */
        little,
        /** big endian */
        big,
    };

    /**
     * @brief Operating system platform type.
     *
     */
    enum class os_platform
    {
        /** Windows */
        os_windows,
        /** macOS */
        os_macos,
        /** Linux */
        os_linux,
        /** Unix */
        os_unix,
        /** Unknown */
        os_unknown,
    };

    /**
     * @brief Get the platform type for the current operating system.
     *
     * @return platform The operating system platform type.
     */
    os_platform platform_type();

    /**
     * @brief Get the architecture of the cpu processor(or operating system).
     *
     * @return std::string the architecture.
     */
    std::string architecture();

    /**
     * @brief Get the platform name for the current operating system.
     *
     * @param type the operating system platform type.
     *
     * @return std::string the platform name.
     */
    std::string platform_name(os_platform type);

    /**
     * @brief Get the C++ standard library version.
     *
     * @return std::string the C++ standard library version.
     */
    std::string cpp_stl_version();

    /**
     * @brief Get the program bit.
     *
     * @return uint16_t the program bit.
     */
    uint16_t program_bit();

    /**
     * @brief Get the program endianness.
     *
     * @return endian the program endianness.
     */
    endian endian_type();
    /**
     * @brief Byteswap a 16-bit value.
     *
     * @param value the value to byteswap.
     * @return uint16_t the byteswapped value.
     */
    uint16_t byteswap(uint16_t value);
    /**
     * @brief Byteswap a 32-bit value.
     *
     * @param value the value to byteswap.
     * @return uint32_t the byteswapped value.
     */
    uint32_t byteswap(uint32_t value);
    /**
     * @brief Byteswap a 64-bit value.
     *
     * @param value the value to byteswap.
     * @return uint64_t the byteswapped value.
     */
    uint64_t byteswap(uint64_t value);
    /**
     * @brief Byteswap an array of bytes.
     *
     * @param data the array of bytes to byteswap.
     * @param size the size of the array.
     */
    void byteswap(uint8_t *data, uint32_t size);

    /**
     * @brief Execute a system command.
     *
     * @param cmd the command to be executed.
     * @return true if the command is executed successfully, false otherwise.
     */
    bool system(const std::string &cmd);
    /**
     * @brief Execute a system command and get the output.
     *
     * @param cmd the command to be executed.
     * @param result the output of the command.
     * @return true if the command is executed successfully, false otherwise.
     */
    bool callcmd(const std::string &cmd, std::string &result);
    /**
     * @brief Get an environment variable.
     *
     * @param name the name of the environment variable.
     * @param default_value the default value if the variable is not found.
     * @return std::string the value of the environment variable.
     */
    std::string getenv(const std::string &name, const std::string &default_value);

    /**
     * @brief Get the working directory of the current excuting process.
     *
     * @return file path of the working directory
     */
    std::string getcwd();

    /**
     * @brief Get the home dir for the current user.
     *
     * @return std::string the home directory.
     */
    std::string homedir();

} // namespace