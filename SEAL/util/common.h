#ifndef SEAL_UTIL_COMMON_H
#define SEAL_UTIL_COMMON_H

#include <cstdint>
#include <stdexcept>

//#define _BIG_ENDIAN
#define _LITTLE_ENDIAN

namespace seal
{
    namespace util
    {
        const int bytes_per_uint64 = sizeof(std::uint64_t);

        const int bytes_per_uint32 = sizeof(std::uint32_t);

        const int uint32_per_uint64 = 2;

        const int bits_per_nibble = 4;

        const int bits_per_byte = 8;

        const int bits_per_uint64 = bytes_per_uint64 * bits_per_byte;

        const int bits_per_uint32 = bytes_per_uint32 * bits_per_byte;

        const int nibbles_per_byte = 2;

        const int nibbles_per_uint64 = bytes_per_uint64 * nibbles_per_byte;

        const std::uint64_t uint64_high_bit = static_cast<std::uint64_t>(1) << (bits_per_uint64 - 1);

        inline uint32_t reverse_bits(uint32_t x)
        {
            x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
            x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
            x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
            x = (((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8));
            return((x >> 16) | (x << 16));
        }

        inline bool is_hex_char(char hex)
        {
            if (hex >= '0' && hex <= '9')
            {
                return true;
            }
            if (hex >= 'A' && hex <= 'F')
            {
                return true;
            }
            if (hex >= 'a' && hex <= 'f')
            {
                return true;
            }
            return false;
        }

        inline char nibble_to_upper_hex(int nibble)
        {
#ifdef _DEBUG
            if (nibble < 0 || nibble > 15)
            {
                throw std::invalid_argument("nibble");
            }
#endif
            if (nibble < 10)
            {
                return nibble + '0';
            }
            return nibble + 'A' - 10;
        }

        inline int hex_to_nibble(char hex)
        {
            if (hex >= '0' && hex <= '9')
            {
                return hex - '0';
            }
            if (hex >= 'A' && hex <= 'F')
            {
                return hex - 'A' + 10;
            }
            if (hex >= 'a' && hex <= 'f')
            {
                return hex - 'a' + 10;
            }
#ifdef _DEBUG
            throw std::invalid_argument("hex");
#endif
            return -1;
        }

        inline int divide_round_up(int value, int divisor)
        {
#ifdef _DEBUG
            if (value < 0)
            {
                throw std::invalid_argument("value");
            }
            if (divisor <= 0)
            {
                throw std::invalid_argument("divisor");
            }
#endif
            return (value + divisor - 1) / divisor;
        }

        inline std::uint8_t *get_uint64_byte(std::uint64_t *value, int byte_index)
        {
#ifdef _DEBUG
            if (value == nullptr)
            {
                throw std::invalid_argument("value");
            }
            if (byte_index < 0)
            {
                throw std::invalid_argument("byte_index");
            }
#endif
#ifdef _BIG_ENDIAN
            int uint64_index = byte_index / bytes_per_uint64;
            int byte_subindex = byte_index % bytes_per_uint64;
            value += uint64_index;
            return reinterpret_cast<std::uint8_t*>(value) + (bytes_per_uint64 - 1) - byte_subindex;
#else
            return reinterpret_cast<std::uint8_t*>(value) + byte_index;
#endif
        }

        inline const std::uint8_t *get_uint64_byte(const std::uint64_t *value, int byte_index)
        {
#ifdef _DEBUG
            if (value == nullptr)
            {
                throw std::invalid_argument("value");
            }
            if (byte_index < 0)
            {
                throw std::invalid_argument("byte_index");
            }
#endif
#ifdef _BIG_ENDIAN
            int uint64_index = byte_index / bytes_per_uint64;
            int byte_subindex = byte_index % bytes_per_uint64;
            value += uint64_index;
            return reinterpret_cast<const std::uint8_t*>(value) + (bytes_per_uint64 - 1) - byte_subindex;
#else
            return reinterpret_cast<const std::uint8_t*>(value) + byte_index;
#endif
        }

        inline std::uint32_t *get_uint64_uint32(std::uint64_t *value, int uint32_index)
        {
#ifdef _DEBUG
            if (value == nullptr)
            {
                throw std::invalid_argument("value");
            }
            if (uint32_index < 0)
            {
                throw std::invalid_argument("uint32_index");
            }
#endif
#ifdef _BIG_ENDIAN
            int uint64_index = uint32_index / uint32_per_uint64;
            int uint32_subindex = uint32_index % uint32_per_uint64;
            value += uint64_index;
            return reinterpret_cast<std::uint32_t*>(value) + (uint32_per_uint64 - 1) - uint32_subindex;
#else
            return reinterpret_cast<std::uint32_t*>(value) + uint32_index;
#endif
        }

        inline const std::uint32_t *get_uint64_uint32(const std::uint64_t *value, int uint32_index)
        {
#ifdef _DEBUG
            if (value == nullptr)
            {
                throw std::invalid_argument("value");
            }
            if (uint32_index < 0)
            {
                throw std::invalid_argument("uint32_index");
            }
#endif
#ifdef _BIG_ENDIAN
            int uint64_index = uint32_index / uint32_per_uint64;
            int uint32_subindex = uint32_index % uint32_per_uint64;
            value += uint64_index;
            return reinterpret_cast<const std::uint32_t*>(value) + (uint32_per_uint64 - 1) - uint32_subindex;
#else
            return reinterpret_cast<const std::uint32_t*>(value) + uint32_index;
#endif
        }

        int get_significant_bit_count(std::uint64_t value);

        std::string uint64_to_hex_string(const std::uint64_t *value, int uint64_count);

        class MemoryPool;
        std::string uint64_to_dec_string(const std::uint64_t *value, int uint64_count, MemoryPool &pool);

        void hex_string_to_uint64(const char *hex_string, int char_count, int uint64_count, std::uint64_t *result);

        int get_hex_string_bit_count(const char *hex_string, int char_count);

        int get_power_of_two(std::uint64_t value);

        int get_power_of_two_minus_one(std::uint64_t value);
    }
}

#endif // SEAL_UTIL_COMMON_H
