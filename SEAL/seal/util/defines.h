#pragma once

// SEAL version
#define SEAL_VERSION_STRING "v2.3.0-4"

// For extended parameter checks compile with -DSEAL_DEBUG in GCC
// or compile in Debug mode in Visual Studio 
#if defined(_MSC_VER) && defined(_DEBUG)
#define SEAL_DEBUG
#endif

// SEAL checks input argument compatibility and validity using hashes of
// the encryption parameters. The validity guarantee is implemented by 
// restricting (direct) access to ciphertext and key data, and to the 
// hashes that these objects store. This works well when implementing 
// high-level applications using SEAL, but sometimes is too rigid for 
// certain lower level applications. SEAL_EXPOSE_MUTABLE_HASH_BLOCK  
// exposes functions to manually edit the parameter hashes to make the 
// parameter compatibility checks pass in cases where they normally 
// should not pass. Please note that it is extremely easy to break 
// things by doing this, and the consequences can be unexpected.
//#define SEAL_EXPOSE_MUTABLE_HASH_BLOCK

// Allow ciphertext data to be directly modified by exposing the
// functions seal::Ciphertext::mutable_pointer(int) and 
// seal::Ciphertext::set_zero(int) in the public API. Most users should 
// have no reason to allow this. Another less extreme and recommended 
// way of mutating ciphertext data is by allocating memory manually, 
// and using aliased ciphertexts pointing to the allocated memory, 
// which can then be mutated freely.
//#define SEAL_EXPOSE_MUTABLE_CIPHERTEXT

// For security reasons one should never throw when decoding fails due
// to overflow, but in some cases this might help in diagnosing problems.
//#define SEAL_THROW_ON_DECODER_OVERFLOW

// Multiplication by a plaintext zero should not be allowed, and by
// default SEAL throws an error in this case. For performance reasons
// one might want to undefine this if appropriate checks are performed
// elsewhere.
#define SEAL_THROW_ON_MULTIPLY_PLAIN_BY_ZERO

// Compile for big-endian system (not implemented)
//#define SEAL_BIG_ENDIAN

// Bound on the bit-length of user-defined moduli
#define SEAL_USER_MODULO_BIT_BOUND 60

// Bound on the number of coefficient moduli
#define SEAL_COEFF_MOD_COUNT_BOUND 62

// Maximum value for decomposition bit count
#define SEAL_DBC_MAX 60

// Minimum value for decomposition bit count
#define SEAL_DBC_MIN 1

// Debugging help
#define SEAL_ASSERT(condition) { if(!(condition)){ std::cerr << "ASSERT FAILED: "   \
    << #condition << " @ " << __FILE__ << " (" << __LINE__ << ")" << std::endl; } }

// String expansion 
#define SEAL_STR(x) #x
#define SEAL_STRX(x) SEAL_STR(x)

// Microsoft Visual Studio 2012 or newer
#if (_MSC_VER >= 1700)

// X64
#ifdef _M_X64

// Use compiler intrinsics for better performance
#define SEAL_ENABLE_INTRIN

#ifdef SEAL_ENABLE_INTRIN
#include <intrin.h>

#pragma intrinsic(_addcarry_u64)
#define SEAL_ADD_CARRY_UINT64(operand1, operand2, carry, result) _addcarry_u64(     \
    carry,                                                                          \
    static_cast<unsigned long long>(operand1),                                      \
    static_cast<unsigned long long>(operand2),                                      \
    reinterpret_cast<unsigned long long*>(result))

#pragma intrinsic(_subborrow_u64)
#define SEAL_SUB_BORROW_UINT64(operand1, operand2, borrow, result) _subborrow_u64(  \
    borrow,                                                                         \
    static_cast<unsigned long long>(operand1),                                      \
    static_cast<unsigned long long>(operand2),                                      \
    reinterpret_cast<unsigned long long*>(result))

#pragma intrinsic(_BitScanReverse64)
#define SEAL_MSB_INDEX_UINT64(result, value) _BitScanReverse64(result, value)

#pragma intrinsic(_umul128)
#define SEAL_MULTIPLY_UINT64(operand1, operand2, result128) {                       \
    result128[0] = _umul128(                                                        \
        static_cast<unsigned long long>(operand1),                                  \
        static_cast<unsigned long long>(operand2),                                  \
        reinterpret_cast<unsigned long long*>(result128 + 1));                      \
}
#define SEAL_MULTIPLY_UINT64_HW64(operand1, operand2, hw64) {                       \
    _umul128(                                                                       \
        static_cast<unsigned long long>(operand1),                                  \
        static_cast<unsigned long long>(operand2),                                  \
        reinterpret_cast<unsigned long long*>(hw64));                               \
}
#endif
#else //_M_X64
#undef SEAL_ENABLE_INTRIN
#endif //_M_X64

#endif //_MSC_VER


// GNU GCC/G++
#if defined(__GNUC__) && (__GNUC__ < 5)
#error "SEAL requires __GNUC__ >= 5" 
#endif

#if (__GNUC__ >= 5) && defined(__cplusplus)

// Read in config.h to disable unavailable intrinsics
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// Are intrinsics enabled?
#ifdef SEAL_ENABLE_INTRIN
#include <x86intrin.h>

#ifdef SEAL_ENABLE___BUILTIN_CLZLL
// Builtin
#define SEAL_MSB_INDEX_UINT64(result, value) {                                      \
    *result = 63 - __builtin_clzll(value);                                          \
}
#endif //SEAL_ENABLE___BUILTIN_CLZLL

#ifdef SEAL_ENABLE___INT128
// Builtin
#define SEAL_MULTIPLY_UINT64_HW64(operand1, operand2, hw64) {                       \
    *hw64 = static_cast<uint64_t>((static_cast<unsigned __int128>(operand1)         \
            * static_cast<unsigned __int128>(operand2)) >> 64);                     \
}
// Builtin
#define SEAL_MULTIPLY_UINT64(operand1, operand2, result128) {                       \
    unsigned __int128 product = static_cast<unsigned __int128>(operand1) * operand2;\
    result128[0] = static_cast<uint64_t>(product);                                  \
    result128[1] = product >> 64;                                                   \
}
#endif //SEAL_ENABLE___INT128

#ifdef SEAL_ENABLE__ADDCARRY_U64
#define SEAL_ADD_CARRY_UINT64(operand1, operand2, carry, result) _addcarry_u64(     \
    carry,                                                                          \
    static_cast<unsigned long long>(operand1),                                      \
    static_cast<unsigned long long>(operand2),                                      \
    reinterpret_cast<unsigned long long*>(result))
#endif //SEAL_ENABLE__ADDCARRY_U64

#ifdef SEAL_ENABLE__SUBBORROW_U64
#if (__GNUC__ == 7) && (__GNUC_MINOR__ >= 2)
// The inverted arguments problem was fixed in GCC-7.2 
// (https://patchwork.ozlabs.org/patch/784309/)
#define SEAL_SUB_BORROW_UINT64(operand1, operand2, borrow, result) _subborrow_u64(  \
    borrow,                                                                         \
    static_cast<unsigned long long>(operand1),                                      \
    static_cast<unsigned long long>(operand2),                                      \
    reinterpret_cast<unsigned long long*>(result))
#else
// Warning: Note the inverted order of operand1 and operand2
#define SEAL_SUB_BORROW_UINT64(operand1, operand2, borrow, result) _subborrow_u64(  \
    borrow,                                                                         \
    static_cast<unsigned long long>(operand2),                                      \
    static_cast<unsigned long long>(operand1),                                      \
    reinterpret_cast<unsigned long long*>(result))
#endif //(__GNUC__ == 7) && (__GNUC_MINOR__ >= 2)
#endif //SEAL_ENABLE__SUBBORROW_U64

#endif //SEAL_ENABLE_INTRIN
#endif //defined(__GNUC__ >= 5) && defined(__cplusplus)

// Use generic functions as (slower) fallback
#ifndef SEAL_ADD_CARRY_UINT64
#define SEAL_ADD_CARRY_UINT64(operand1, operand2, carry, result) add_uint64_generic(operand1, operand2, carry, result)
//#pragma message("SEAL_ADD_CARRY_UINT64 not defined. Using add_uint64_generic (see util/defines.h)")
#endif

#ifndef SEAL_SUB_BORROW_UINT64
#define SEAL_SUB_BORROW_UINT64(operand1, operand2, borrow, result) sub_uint64_generic(operand1, operand2, borrow, result)
//#pragma message("SEAL_SUB_BORROW_UINT64 not defined. Using sub_uint64_generic (see util/defines.h).")
#endif

#ifndef SEAL_MULTIPLY_UINT64
#define SEAL_MULTIPLY_UINT64(operand1, operand2, result128) {                      \
    multiply_uint64_generic(operand1, operand2, result128);                        \
}
//#pragma message("SEAL_MULTIPLY_UINT64 not defined. Using multiply_uint64_generic (see util/defines.h).")
#endif

#ifndef SEAL_MULTIPLY_UINT64_HW64
#define SEAL_MULTIPLY_UINT64_HW64(operand1, operand2, hw64) {                      \
    multiply_uint64_hw64_generic(operand1, operand2, hw64);                        \
}
//#pragma message("SEAL_MULTIPLY_UINT64 not defined. Using multiply_uint64_generic (see util/defines.h).")
#endif

#ifndef SEAL_MSB_INDEX_UINT64
#define SEAL_MSB_INDEX_UINT64(result, value) get_msb_index_generic(result, value)
//#pragma message("SEAL_MSB_INDEX_UINT64 not defined. Using get_msb_index_generic (see util/defines.h).")
#endif
