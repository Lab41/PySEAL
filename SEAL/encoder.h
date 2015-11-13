#ifndef SEAL_ENCODER_H
#define SEAL_ENCODER_H

#include <cstdint>
#include "biguint.h"
#include "bigpoly.h"
#include "util/mempool.h"

namespace seal
{
    class BinaryEncoder
    {
    public:
        BinaryEncoder(BigUInt plain_modulus);

        BigPoly encode(std::uint64_t value);

        void encode(std::uint64_t value, BigPoly &destination);

        std::uint32_t decode_uint32(const BigPoly &poly);

        std::uint64_t decode_uint64(const BigPoly &poly);

        BigPoly encode(std::int64_t value);

        void encode(std::int64_t value, BigPoly &destination);

        BigPoly encode(BigUInt value);

        void encode(BigUInt value, BigPoly &destination);

        std::int32_t decode_int32(const BigPoly &poly);

        std::int64_t decode_int64(const BigPoly &poly);

        BigUInt decode_biguint(const BigPoly &poly);

        void decode_biguint(const BigPoly &poly, BigUInt &destination);

        // To prevent problems with automatic type conversion from int32_t
        BigPoly encode(std::int32_t value)
        {
            return encode(static_cast<std::int64_t>(value));
        }

        // To prevent problems with automatic type conversion from uint32_t
        BigPoly encode(std::uint32_t value)
        {
            return encode(static_cast<std::uint64_t>(value));
        }

        const BigUInt &plain_modulus() const
        {
            return plain_modulus_;
        }

        void set_plain_modulus(BigUInt &value);

    private:
        BigUInt plain_modulus_;

        BigUInt coeff_neg_threshold_;

        BigUInt neg_one_;

        util::MemoryPool pool_;

        BinaryEncoder(const BinaryEncoder &copy) = delete;

        BinaryEncoder &operator =(const BinaryEncoder &assign) = delete;
    };

    class BalancedEncoder
    {
    public:
        BalancedEncoder(BigUInt plain_modulus, int base = 3);

        BigPoly encode(std::uint64_t value);

        void encode(std::uint64_t value, BigPoly &destination);

        std::uint32_t decode_uint32(const BigPoly &poly);

        std::uint64_t decode_uint64(const BigPoly &poly);

        BigPoly encode(std::int64_t value);

        void encode(std::int64_t value, BigPoly &destination);

        BigPoly encode(BigUInt value);

        void encode(BigUInt value, BigPoly &destination);

        std::int32_t decode_int32(const BigPoly &poly);

        std::int64_t decode_int64(const BigPoly &poly);

        BigUInt decode_biguint(const BigPoly &poly);

        void decode_biguint(const BigPoly &poly, BigUInt &destination);

        // To prevent problems with automatic type conversion from int32_t
        BigPoly encode(std::int32_t value)
        {
            return encode(static_cast<std::int64_t>(value));
        }

        // To prevent problems with automatic type conversion from uint32_t
        BigPoly encode(std::uint32_t value)
        {
            return encode(static_cast<std::uint64_t>(value));
        }

        const BigUInt &plain_modulus() const
        {
            return plain_modulus_;
        }

        int base() const
        {
            return base_;
        }

        void set_plain_modulus(BigUInt &value);

        void set_base(int base);

    private:
        BigUInt plain_modulus_;

        int base_;

        BigUInt coeff_neg_threshold_;

        util::MemoryPool pool_;

        BalancedEncoder(const BalancedEncoder &copy) = delete;

        BalancedEncoder &operator =(const BalancedEncoder &assign) = delete;
    };

    class BinaryFractionalEncoder
    {
    public:
        BinaryFractionalEncoder(const BigUInt &plain_modulus, const BigPoly &poly_modulus, int integer_coeff_count, int fraction_coeff_count);

        BigPoly encode(double value);

        double decode(const BigPoly &value);

        const BigUInt &plain_modulus() const
        {
            return encoder_.plain_modulus();
        }

        void set_plain_modulus(BigUInt &value)
        {
            encoder_.set_plain_modulus(value);
        }

    private:
        BinaryFractionalEncoder(const BinaryFractionalEncoder &copy) = delete;

        BinaryFractionalEncoder &operator =(const BinaryFractionalEncoder &assign) = delete;

        BinaryEncoder encoder_;

        int fraction_coeff_count_;

        int integer_coeff_count_;

        BigPoly poly_modulus_;
    };

    class BalancedFractionalEncoder
    {
    public:
        BalancedFractionalEncoder(const BigUInt &plain_modulus, const BigPoly &poly_modulus, int integer_coeff_count, int fraction_coeff_count, int base = 3);

        BigPoly encode(double value);

        double decode(const BigPoly &value);

        const BigUInt &plain_modulus() const
        {
            return encoder_.plain_modulus();
        }

        int base() const
        {
            return encoder_.base();
        }

        void set_plain_modulus(BigUInt &value)
        {
            encoder_.set_plain_modulus(value);
        }

        void set_base(int value)
        {
            encoder_.set_base(value);
        }

    private:
        BalancedFractionalEncoder(const BalancedFractionalEncoder &copy) = delete;

        BalancedFractionalEncoder &operator =(const BalancedFractionalEncoder &assign) = delete;

        BalancedEncoder encoder_;

        int fraction_coeff_count_;

        int integer_coeff_count_;

        BigPoly poly_modulus_;
    };
}

#endif // SEAL_ENCODER_H
