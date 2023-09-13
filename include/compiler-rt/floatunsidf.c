
#define DOUBLE_PRECISION
#include "fp_lib.h"

COMPILER_RT_ABI fp_t __floatunsidf(su_int a) {
    const int aWidth = sizeof a * CHAR_BIT;

    // Handle zero as a special case to protect clz
    if (a == 0) return fromRep(0);

    // Exponent of (fp_t)a is the width of abs(a).
    const int exponent = (aWidth - 1) - clzsi(a);
    rep_t result;

    // Shift a into the significand field and clear the implicit bit.
    const int shift = significandBits - exponent;
    result = (rep_t)a << shift ^ implicitBit;

    // Insert the exponent
    result += (rep_t)(exponent + exponentBias) << significandBits;
    return fromRep(result);
}
