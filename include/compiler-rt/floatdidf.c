#define DOUBLE_PRECISION
#include "fp_lib.h"

COMPILER_RT_ABI double __floatdidf(di_int a) {
    static const double twop52 = 4503599627370496.0;  // 0x1.0p52
    static const double twop32 = 4294967296.0;        // 0x1.0p32

    union {
        int64_t x;
        double d;
    } low = {.d = twop52};

    const double high = (int32_t)(a >> 32) * twop32;
    low.x |= a & INT64_C(0x00000000ffffffff);

    const double result = (high - twop52) + low.d;
    return result;
}
