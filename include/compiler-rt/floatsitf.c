
#define QUAD_PRECISION
#include "fp_lib.h"
COMPILER_RT_ABI fp_t __floatsitf(si_int a) {
    const int aWidth = sizeof a * CHAR_BIT;

    // Handle zero as a special case to protect clz
    if (a == 0) return fromRep(0);

    // All other cases begin by extracting the sign and absolute value of a
    rep_t sign = 0;
    su_int aAbs = (su_int)a;
    if (a < 0) {
        sign = signBit;
        aAbs = ~(su_int)a + (su_int)1U;
    }

    // Exponent of (fp_t)a is the width of abs(a).
    const int exponent = (aWidth - 1) - clzsi(aAbs);
    rep_t result;

    // Shift a into the significand field and clear the implicit bit.
    const int shift = significandBits - exponent;
    result = (rep_t)aAbs << shift ^ implicitBit;

    // Insert the exponent
    result += (rep_t)(exponent + exponentBias) << significandBits;
    // Insert the sign bit and return
    return fromRep(result | sign);
}
