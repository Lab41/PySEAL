#pragma once

// Disable the use of NTT in homomorphic multiplication. If this is defined, the encryption
// parameter aux_coeff_modulus has no effect, and NTT will never be used in Evaluator::multiply
// or Evaluator::square.
#define DISABLE_NTT_IN_MULTIPLY

// For security reasons one should never throw when decoding fails due to overflow, but in
// some cases this might help in diagnosing problems.
#undef THROW_ON_DECODER_OVERFLOW

// Microsoft Visual Studio 2012 or newer
#if (_MSC_VER >= 1700)

// X64
#ifdef _M_X64

// Use compiler intrinsics for better performance
#define ENABLE_INTRIN

#include <intrin.h>

#pragma intrinsic(_addcarry_u64)
#define ADD_CARRY_UINT64(operand1, operand2, carry, result) _addcarry_u64(          \
    carry,                                                                          \
    static_cast<unsigned long long>(operand1),                                      \
    static_cast<unsigned long long>(operand2),                                      \
    reinterpret_cast<unsigned long long*>(result))

#pragma intrinsic(_subborrow_u64)
#define SUB_BORROW_UINT64(operand1, operand2, borrow, result) _subborrow_u64(       \
    borrow,                                                                         \
    static_cast<unsigned long long>(operand1),                                      \
    static_cast<unsigned long long>(operand2),                                      \
    reinterpret_cast<unsigned long long*>(result))

#pragma intrinsic(_BitScanReverse64)
#define MSB_INDEX_UINT64(result, value) _BitScanReverse64(result, value)

#pragma intrinsic(_umul128)
#define MULTIPLY_UINT64(operand1, operand2, carry) _umul128(                        \
    static_cast<unsigned long long>(operand1),                                      \
    static_cast<unsigned long long>(operand2),                                      \
    reinterpret_cast<unsigned long long*>(carry))

#else //_M_X64

#undef ENABLE_INTRIN

#endif //_M_X64

#endif //_MSC_VER


// GNU GCC/G++
#if (__GNUC__ >= 5) && defined(__cplusplus)

// Read in config.h to disable unavailable intrinsics
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// Are intrinsics enabled?
#ifdef ENABLE_INTRIN
#include <x86intrin.h>

#ifdef ENABLE___BUILTIN_CLZLL
// __builtin
#define MSB_INDEX_UINT64(result, value) {                                           \
    *result = 63 - __builtin_clzll(value);                                          \
}
#endif //ENABLE___BUILTIN_CLZLL

#ifdef ENABLE__ADDCARRY_U64
#define ADD_CARRY_UINT64(operand1, operand2, carry, result) _addcarry_u64(          \
    carry,                                                                          \
    static_cast<unsigned long long>(operand1),                                      \
    static_cast<unsigned long long>(operand2),                                      \
    reinterpret_cast<unsigned long long*>(result))
#endif //ENABLE__ADDCARRY_U64

#ifdef ENABLE__SUBBORROW_U64
// Warning: Note the inverted order of operand1 and operand2
#define SUB_BORROW_UINT64(operand1, operand2, borrow, result) _subborrow_u64(       \
    borrow,                                                                         \
    static_cast<unsigned long long>(operand2),                                      \
    static_cast<unsigned long long>(operand1),                                      \
    reinterpret_cast<unsigned long long*>(result))
#endif //ENABLE__SUBBORROW_U64

#ifdef ENABLE__MULX_U64
#define MULTIPLY_UINT64(operand1, operand2, carry) _mulx_u64(                       \
    static_cast<unsigned long long>(operand1),                                      \
    static_cast<unsigned long long>(operand2),                                      \
    reinterpret_cast<unsigned long long*>(carry))
#endif //ENABLE__MULX_U64

#endif //ENABLE_INTRIN

#endif //(defined(__GNUC__) || defined(__GNUG__))


// Use generic functions as (slower) fallback
#ifndef ADD_CARRY_UINT64
#define ADD_CARRY_UINT64(operand1, operand2, carry, result) add_uint64_uint64_generic(operand1, operand2, carry, result)
//#pragma message("ADD_CARRY_UINT64 not defined. Using add_uint64_uint64_generic (see util/defines.h)")
#endif

#ifndef SUB_BORROW_UINT64
#define SUB_BORROW_UINT64(operand1, operand2, borrow, result) sub_uint64_uint64_generic(operand1, operand2, borrow, result)
//#pragma message("SUB_BORROW_UINT64 not defined. Using sub_uint64_uint64_generic (see util/defines.h).")
#endif

#ifndef MULTIPLY_UINT64
#define MULTIPLY_UINT64(operand1, operand2, carry) multiply_uint64_uint64_generic(operand1, operand2, carry)
//#pragma message("MULTIPLY_UINT64 not defined. Using multiply_uint64_uint64_generic (see util/defines.h).")
#endif

#ifndef MSB_INDEX_UINT64
#define MSB_INDEX_UINT64(result, value) get_msb_index_generic(result, value)
//#pragma message("MSB_INDEX_UINT64 not defined. Using get_msb_index_generic (see util/defines.h).")
#endif