//===-- lib/fp_lib.h - Floating-point utilities -------------------*- C -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file is a configuration header for soft-float routines in compiler-rt.
// This file does not provide any part of the compiler-rt interface, but defines
// many useful constants and utility routines that are used in the
// implementation of the soft-float routines in compiler-rt.
//
// Assumes that float, double and long double correspond to the IEEE-754
// binary32, binary64 and binary 128 types, respectively, and that integer
// endianness matches floating point endianness on the target platform.
//
//===----------------------------------------------------------------------===//

#ifndef FP_LIB_HEADER
#define FP_LIB_HEADER

// #include "int_lib.h"
// #include "int_math.h"
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>

#define UINT8_C(c) c
#define UINT16_C(c) c
#define UINT32_C(c) c##U
#define clzsi __builtin_clz
typedef int32_t si_int;
typedef uint32_t su_int;
typedef int64_t di_int;
typedef uint64_t du_int;

//
#define COMPILER_RT_ABI
#define COMPILER_RT_ALIAS(name, aliasname) \
    COMPILER_RT_ABI __typeof(name) aliasname __attribute__((__alias__(#name)));

#define UINT64_C(c) (c##ULL)
#define crt_isnan(x) __builtin_isnan((x))

typedef uint32_t half_rep_t;
typedef uint64_t rep_t;
typedef int64_t srep_t;
typedef double fp_t;
#define HALF_REP_C UINT32_C
#define REP_C UINT64_C
#define significandBits 52

static __inline int rep_clz(rep_t a) {
#if defined __LP64__
    return __builtin_clzl(a);
#else
    if (a & REP_C(0xffffffff00000000))
        return clzsi(a >> 32);
    else
        return 32 + clzsi(a & REP_C(0xffffffff));
#endif
}

#define loWord(a) (a & 0xffffffffU)
#define hiWord(a) (a >> 32)

// 64x64 -> 128 wide multiply for platforms that don't have such an operation;
// many 64-bit platforms have this operation, but they tend to have hardware
// floating-point, so we don't bother with a special case for them here.
static __inline void wideMultiply(rep_t a, rep_t b, rep_t *hi, rep_t *lo) {
    // Each of the component 32x32 -> 64 products
    const uint64_t plolo = loWord(a) * loWord(b);
    const uint64_t plohi = loWord(a) * hiWord(b);
    const uint64_t philo = hiWord(a) * loWord(b);
    const uint64_t phihi = hiWord(a) * hiWord(b);
    // Sum terms that contribute to lo in a way that allows us to get the carry
    const uint64_t r0 = loWord(plolo);
    const uint64_t r1 = hiWord(plolo) + loWord(plohi) + loWord(philo);
    *lo = r0 + (r1 << 32);
    // Sum terms contributing to hi with the carry from lo
    *hi = hiWord(plohi) + hiWord(philo) + hiWord(r1) + phihi;
}
#undef loWord
#undef hiWord

COMPILER_RT_ABI fp_t __adddf3(fp_t a, fp_t b);

#define typeWidth (sizeof(rep_t) * CHAR_BIT)
#define exponentBits (typeWidth - significandBits - 1)
#define maxExponent ((1 << exponentBits) - 1)
#define exponentBias (maxExponent >> 1)

#define implicitBit (REP_C(1) << significandBits)
#define significandMask (implicitBit - 1U)
#define signBit (REP_C(1) << (significandBits + exponentBits))
#define absMask (signBit - 1U)
#define exponentMask (absMask ^ significandMask)
#define oneRep ((rep_t)exponentBias << significandBits)
#define infRep exponentMask
#define quietBit (implicitBit >> 1)
#define qnanRep (exponentMask | quietBit)

static __inline rep_t toRep(fp_t x) {
    const union {
        fp_t f;
        rep_t i;
    } rep = {.f = x};
    return rep.i;
}

static __inline fp_t fromRep(rep_t x) {
    const union {
        fp_t f;
        rep_t i;
    } rep = {.i = x};
    return rep.f;
}

static __inline int normalize(rep_t *significand) {
    const int shift = rep_clz(*significand) - rep_clz(implicitBit);
    *significand <<= shift;
    return 1 - shift;
}

static __inline void wideLeftShift(rep_t *hi, rep_t *lo, int count) {
    *hi = *hi << count | *lo >> (typeWidth - count);
    *lo = *lo << count;
}

static __inline void wideRightShiftWithSticky(rep_t *hi, rep_t *lo,
                                              unsigned int count) {
    if (count < typeWidth) {
        const bool sticky = (*lo << (typeWidth - count)) != 0;
        *lo = *hi << (typeWidth - count) | *lo >> count | sticky;
        *hi = *hi >> count;
    } else if (count < 2 * typeWidth) {
        const bool sticky = *hi << (2 * typeWidth - count) | *lo;
        *lo = *hi >> (count - typeWidth) | sticky;
        *hi = 0;
    } else {
        const bool sticky = *hi | *lo;
        *lo = sticky;
        *hi = 0;
    }
}

// Implements logb methods (logb, logbf, logbl) for IEEE-754. This avoids
// pulling in a libm dependency from compiler-rt, but is not meant to replace
// it (i.e. code calling logb() should get the one from libm, not this), hence
// the __compiler_rt prefix.
static __inline fp_t __compiler_rt_logbX(fp_t x) {
    rep_t rep = toRep(x);
    int exp = (rep & exponentMask) >> significandBits;

    // Abnormal cases:
    // 1) +/- inf returns +inf; NaN returns NaN
    // 2) 0.0 returns -inf
    if (exp == maxExponent) {
        if (((rep & signBit) == 0) || (x != x)) {
            return x;  // NaN or +inf: return x
        } else {
            return -x;  // -inf: return -x
        }
    } else if (x == 0.0) {
        // 0.0: return -inf
        return fromRep(infRep | signBit);
    }

    if (exp != 0) {
        // Normal number
        return exp - exponentBias;  // Unbias exponent
    } else {
        // Subnormal number; normalize and repeat
        rep &= absMask;
        const int shift = 1 - normalize(&rep);
        exp = (rep & exponentMask) >> significandBits;
        return exp - exponentBias - shift;  // Unbias exponent
    }
}

// Avoid using scalbn from libm. Unlike libc/libm scalbn, this function never
// sets errno on underflow/overflow.
static __inline fp_t __compiler_rt_scalbnX(fp_t x, int y) {
    const rep_t rep = toRep(x);
    int exp = (rep & exponentMask) >> significandBits;

    if (x == 0.0 || exp == maxExponent)
        return x;  // +/- 0.0, NaN, or inf: return x

    // Normalize subnormal input.
    rep_t sig = rep & significandMask;
    if (exp == 0) {
        exp += normalize(&sig);
        sig &= ~implicitBit;  // clear the implicit bit again
    }

    if (__builtin_sadd_overflow(exp, y, &exp)) {
        // Saturate the exponent, which will guarantee an underflow/overflow
        // below.
        exp = (y >= 0) ? INT_MAX : INT_MIN;
    }

    // Return this value: [+/-] 1.sig * 2 ** (exp - exponentBias).
    const rep_t sign = rep & signBit;
    if (exp >= maxExponent) {
        // Overflow, which could produce infinity or the largest-magnitude
        // value, depending on the rounding mode.
        return fromRep(sign | ((rep_t)(maxExponent - 1) << significandBits)) *
               2.0f;
    } else if (exp <= 0) {
        // Subnormal or underflow. Use floating-point multiply to handle
        // truncation correctly.
        fp_t tmp = fromRep(sign | (REP_C(1) << significandBits) | sig);
        exp += exponentBias - 1;
        if (exp < 1) exp = 1;
        tmp *= fromRep((rep_t)exp << significandBits);
        return tmp;
    } else
        return fromRep(sign | ((rep_t)exp << significandBits) | sig);
}

// Avoid using fmax from libm.
static __inline fp_t __compiler_rt_fmaxX(fp_t x, fp_t y) {
    // If either argument is NaN, return the other argument. If both are NaN,
    // arbitrarily return the second one. Otherwise, if both arguments are +/-0,
    // arbitrarily return the first one.
    return (crt_isnan(x) || x < y) ? y : x;
}

static __inline fp_t __compiler_rt_logb(fp_t x) {
    return __compiler_rt_logbX(x);
}
static __inline fp_t __compiler_rt_scalbn(fp_t x, int y) {
    return __compiler_rt_scalbnX(x, y);
}
static __inline fp_t __compiler_rt_fmax(fp_t x, fp_t y) {
#if defined(__aarch64__)
    // Use __builtin_fmax which turns into an fmaxnm instruction on AArch64.
    return __builtin_fmax(x, y);
#else
    // __builtin_fmax frequently turns into a libm call, so inline the function.
    return __compiler_rt_fmaxX(x, y);
#endif
}

#endif  // FP_LIB_HEADER
