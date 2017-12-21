#pragma once
 
#include <cstdint>
#include <array>

namespace seal
{
    namespace util
    {
        class HashFunction
        {
        public:
            HashFunction() = delete;

            static const int sha3_block_uint64_count = 4;

            typedef std::array<std::uint64_t, sha3_block_uint64_count> sha3_block_type;

            static const sha3_block_type sha3_zero_block;

            static void sha3_hash(const std::uint64_t *input, int uint64_count, sha3_block_type &destination);

            inline static void sha3_hash(std::uint64_t input, sha3_block_type &destination)
            {
                sha3_hash(&input, 1, destination);
            }

        private:
            static const std::uint8_t sha3_round_count = 24;

            static const std::uint8_t sha3_rate_uint64_count = 17; // Rate 1088 = 17 * 64 bits

            static const std::uint8_t sha3_capacity_uint64_count = 8; // Capacity 512 = 8 * 64 bits

            static const std::uint8_t sha3_state_uint64_count = 25; // State size = 1600 = 25 * 64 bits

            typedef std::uint64_t sha3_state_type[5][5];

            static const std::uint8_t rho[24];

            static const std::uint64_t sha3_round_consts[sha3_round_count];

            inline static std::uint64_t rot(std::uint64_t input, std::uint8_t s)
            {
                return (input << s) | (input >> (64 - s));
            }

            static void keccak_1600(sha3_state_type &state);

            inline static void sponge_absorb(const std::uint64_t sha3_block[sha3_rate_uint64_count], sha3_state_type &state)
            {
                //for (std::uint8_t x = 0; x < 5; x++)
                //{
                //    for (std::uint8_t y = 0; y < 5; y++)
                //    {
                //        std::uint8_t index = 5 * y + x;
                //        state[x][y] ^= index < sha3_rate_uint64_count ? sha3_block[index] : 0ULL;
                //    }
                //}

                state[0][0] ^= 0 < sha3_rate_uint64_count ? sha3_block[0] : 0ULL;
                state[0][1] ^= 5 < sha3_rate_uint64_count ? sha3_block[5] : 0ULL;
                state[0][2] ^= 10 < sha3_rate_uint64_count ? sha3_block[10] : 0ULL;
                state[0][3] ^= 15 < sha3_rate_uint64_count ? sha3_block[15] : 0ULL;
                state[0][4] ^= 20 < sha3_rate_uint64_count ? sha3_block[20] : 0ULL;

                state[1][0] ^= 1 < sha3_rate_uint64_count ? sha3_block[1] : 0ULL;
                state[1][1] ^= 6 < sha3_rate_uint64_count ? sha3_block[6] : 0ULL;
                state[1][2] ^= 11 < sha3_rate_uint64_count ? sha3_block[11] : 0ULL;
                state[1][3] ^= 16 < sha3_rate_uint64_count ? sha3_block[16] : 0ULL;
                state[1][4] ^= 21 < sha3_rate_uint64_count ? sha3_block[21] : 0ULL;

                state[2][0] ^= 2 < sha3_rate_uint64_count ? sha3_block[2] : 0ULL;
                state[2][1] ^= 7 < sha3_rate_uint64_count ? sha3_block[7] : 0ULL;
                state[2][2] ^= 12 < sha3_rate_uint64_count ? sha3_block[12] : 0ULL;
                state[2][3] ^= 17 < sha3_rate_uint64_count ? sha3_block[17] : 0ULL;
                state[2][4] ^= 22 < sha3_rate_uint64_count ? sha3_block[22] : 0ULL;

                state[3][0] ^= 3 < sha3_rate_uint64_count ? sha3_block[3] : 0ULL;
                state[3][1] ^= 8 < sha3_rate_uint64_count ? sha3_block[8] : 0ULL;
                state[3][2] ^= 13 < sha3_rate_uint64_count ? sha3_block[13] : 0ULL;
                state[3][3] ^= 18 < sha3_rate_uint64_count ? sha3_block[18] : 0ULL;
                state[3][4] ^= 23 < sha3_rate_uint64_count ? sha3_block[23] : 0ULL;

                state[4][0] ^= 4 < sha3_rate_uint64_count ? sha3_block[4] : 0ULL;
                state[4][1] ^= 9 < sha3_rate_uint64_count ? sha3_block[9] : 0ULL;
                state[4][2] ^= 14 < sha3_rate_uint64_count ? sha3_block[14] : 0ULL;
                state[4][3] ^= 19 < sha3_rate_uint64_count ? sha3_block[19] : 0ULL;
                state[4][4] ^= 24 < sha3_rate_uint64_count ? sha3_block[24] : 0ULL;

                keccak_1600(state);
            }

            inline static void sponge_squeeze(const sha3_state_type &sha3_state, sha3_block_type &sha3_block)
            {
                // Trivial in this case: we simply output the first blocks of the state
                static_assert(sha3_block_uint64_count == 4, "sha3_block_uint64_count must equal 4");

                sha3_block[0] = sha3_state[0][0];
                sha3_block[1] = sha3_state[1][0];
                sha3_block[2] = sha3_state[2][0];
                sha3_block[3] = sha3_state[3][0];
            }
        };
    }
}