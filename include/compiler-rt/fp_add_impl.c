//===----- lib/fp_add_impl.inc - floaing point addition -----------*- C -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements soft-float addition with the IEEE-754 default rounding
// (to nearest, ties to even).
//
//===----------------------------------------------------------------------===//
#define DOUBLE_PRECISION

#include "fp_lib.h"
#include "fp_add_impl.inc"

COMPILER_RT_ABI double __adddf3(double a, double b) { return __addXf3__(a, b); }

// Subtraction; flip the sign bit of b and add.
COMPILER_RT_ABI fp_t __subdf3(fp_t a, fp_t b) {
    return __adddf3(a, fromRep(toRep(b) ^ signBit));
}
