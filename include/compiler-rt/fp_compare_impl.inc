//===-- lib/fp_compare_impl.inc - Floating-point comparison -------*- C -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
#define DOUBLE_PRECISION

#include "fp_lib.h"

typedef long CMP_RESULT;

enum {
    LE_LESS = -1,
    LE_EQUAL = 0,
    LE_GREATER = 1,
    LE_UNORDERED = 1,
};

static inline CMP_RESULT __leXf2__(fp_t a, fp_t b) {
    const srep_t aInt = toRep(a);
    const srep_t bInt = toRep(b);
    const rep_t aAbs = aInt & absMask;
    const rep_t bAbs = bInt & absMask;

    // If either a or b is NaN, they are unordered.
    if (aAbs > infRep || bAbs > infRep) return LE_UNORDERED;

    // If a and b are both zeros, they are equal.
    if ((aAbs | bAbs) == 0) return LE_EQUAL;

    // If at least one of a and b is positive, we get the same result comparing
    // a and b as signed integers as we would with a floating-point compare.
    if ((aInt & bInt) >= 0) {
        if (aInt < bInt)
            return LE_LESS;
        else if (aInt == bInt)
            return LE_EQUAL;
        else
            return LE_GREATER;
    } else {
        // Otherwise, both are negative, so we need to flip the sense of the
        // comparison to get the correct result.  (This assumes a twos- or ones-
        // complement integer representation; if integers are represented in a
        // sign-magnitude representation, then this flip is incorrect).
        if (aInt > bInt)
            return LE_LESS;
        else if (aInt == bInt)
            return LE_EQUAL;
        else
            return LE_GREATER;
    }
}

enum {
    GE_LESS = -1,
    GE_EQUAL = 0,
    GE_GREATER = 1,
    GE_UNORDERED = -1  // Note: different from LE_UNORDERED
};

static inline CMP_RESULT __geXf2__(fp_t a, fp_t b) {
    const srep_t aInt = toRep(a);
    const srep_t bInt = toRep(b);
    const rep_t aAbs = aInt & absMask;
    const rep_t bAbs = bInt & absMask;

    if (aAbs > infRep || bAbs > infRep) return GE_UNORDERED;
    if ((aAbs | bAbs) == 0) return GE_EQUAL;
    if ((aInt & bInt) >= 0) {
        if (aInt < bInt)
            return GE_LESS;
        else if (aInt == bInt)
            return GE_EQUAL;
        else
            return GE_GREATER;
    } else {
        if (aInt > bInt)
            return GE_LESS;
        else if (aInt == bInt)
            return GE_EQUAL;
        else
            return GE_GREATER;
    }
}

static inline CMP_RESULT __unordXf2__(fp_t a, fp_t b) {
    const rep_t aAbs = toRep(a) & absMask;
    const rep_t bAbs = toRep(b) & absMask;
    return aAbs > infRep || bAbs > infRep;
}
