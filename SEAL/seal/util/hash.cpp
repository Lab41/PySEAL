#include "seal/util/hash.h"
#include "seal/util/common.h"
#include "seal/util/mempool.h"
#include "seal/util/globals.h"
#include <cstring>

using namespace std;

namespace seal
{
    namespace util
    {
        const HashFunction::sha3_block_type HashFunction::sha3_zero_block{ 0 };

        const uint8_t HashFunction::rho[24]{
            1, 3, 6, 10, 15, 21,
            28, 36, 45, 55, 2, 14,
            27, 41, 56, 8, 25, 43,
            62, 18, 39, 61, 20, 44
        };

        const uint64_t HashFunction::sha3_round_consts[sha3_round_count]{
            0x0000000000000001, 0x0000000000008082, 0x800000000000808a,
            0x8000000080008000, 0x000000000000808b, 0x0000000080000001,
            0x8000000080008081, 0x8000000000008009, 0x000000000000008a,
            0x0000000000000088, 0x0000000080008009, 0x000000008000000a,
            0x000000008000808b, 0x800000000000008b, 0x8000000000008089,
            0x8000000000008003, 0x8000000000008002, 0x8000000000000080,
            0x000000000000800a, 0x800000008000000a, 0x8000000080008081,
            0x8000000000008080, 0x0000000080000001, 0x8000000080008008
        };

        void HashFunction::keccak_1600(sha3_state_type &state)
        {
            for (uint8_t round = 0; round < sha3_round_count; round++)
            {
                // theta
                uint64_t C[5];
                uint64_t D[5];
                for (uint8_t x = 0; x < 5; x++)
                {
                    C[x] = state[x][0];
                    for (uint8_t y = 1; y < 5; y++)
                    {
                        C[x] ^= state[x][y];
                    }
                }
                for (uint8_t x = 0; x < 5; x++)
                {
                    D[x] = C[(x + 4) % 5] ^ rot(C[(x + 1) % 5], 1);
                    for (uint8_t y = 0; y < 5; y++)
                    {
                        state[x][y] ^= D[x];
                    }
                }

                // rho and pi
                uint64_t ind_x = 1;
                uint64_t ind_y = 0;
                uint64_t curr = state[ind_x][ind_y];
                for (uint8_t i = 0; i < 24; i++)
                {
                    uint64_t ind_X = ind_y;
                    uint64_t ind_Y = (2 * ind_x + 3 * ind_y) % 5;
                    uint64_t temp = state[ind_X][ind_Y];
                    state[ind_X][ind_Y] = rot(curr, rho[i]);
                    curr = temp;
                    ind_x = ind_X;
                    ind_y = ind_Y;
                }

                // xi
                for (uint8_t y = 0; y < 5; y++)
                {
                    for (uint8_t x = 0; x < 5; x++)
                    {
                        C[x] = state[x][y];
                    }
                    for (uint8_t x = 0; x < 5; x++)
                    {
                        state[x][y] = C[x] ^ ((~C[(x + 1) % 5]) & C[(x + 2) % 5]);
                    }
                }

                // iota
                state[0][0] ^= sha3_round_consts[round];
            }
        }

        void HashFunction::sha3_hash(const uint64_t *input, int uint64_count, sha3_block_type &sha3_block)
        {
#ifdef SEAL_DEBUG
            if (input == nullptr)
            {
                throw invalid_argument("input cannot be null");
            }
            if (uint64_count < 0)
            {
                throw invalid_argument("uint64_count cannot be negative");
            }
#endif
            // Padding
            int padded_uint64_count = sha3_rate_uint64_count * ((uint64_count / sha3_rate_uint64_count) + 1);
            Pointer padded_input(global_variables::global_memory_pool->get_for_uint64_count(padded_uint64_count));
            memcpy(padded_input.get(), input, uint64_count * bytes_per_uint64);
            for (int i = uint64_count; i < padded_uint64_count; i++)
            {
                padded_input[i] = 0;
                if (i == uint64_count)
                {
                    padded_input[i] |= 0x6ULL;
                }
                if (i == padded_uint64_count - 1)
                {
                    padded_input[i] |= 0x1ULL << 63;
                }
            }

            // Absorb
            sha3_state_type state;
            memset(state, 0, sha3_state_uint64_count * bytes_per_uint64);
            for (int i = 0; i < padded_uint64_count; i += sha3_rate_uint64_count)
            {
                sponge_absorb(padded_input.get() + i, state);
            }

            sha3_block = sha3_zero_block;
            sponge_squeeze(state, sha3_block);
        }
    }
}
