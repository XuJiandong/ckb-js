#define DOUBLE_PRECISION
#include "fp_lib.h"
typedef su_int fixuint_t;

static __inline fixuint_t __fixuint(fp_t a) {
    // Break a into sign, exponent, significand parts.
    const rep_t aRep = toRep(a);
    const rep_t aAbs = aRep & absMask;
    const int sign = aRep & signBit ? -1 : 1;
    const int exponent = (aAbs >> significandBits) - exponentBias;
    const rep_t significand = (aAbs & significandMask) | implicitBit;

    // If either the value or the exponent is negative, the result is zero.
    if (sign == -1 || exponent < 0) return 0;

    // If the value is too large for the integer type, saturate.
    if ((unsigned)exponent >= sizeof(fixuint_t) * CHAR_BIT)
        return ~(fixuint_t)0;

    // If 0 <= exponent < significandBits, right shift to get the result.
    // Otherwise, shift left.
    if (exponent < significandBits)
        return significand >> (significandBits - exponent);
    else
        return (fixuint_t)significand << (exponent - significandBits);
}

COMPILER_RT_ABI su_int __fixunsdfsi(fp_t a) { return __fixuint(a); }
