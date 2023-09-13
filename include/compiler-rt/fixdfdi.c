#define DOUBLE_PRECISION
#include "fp_lib.h"

COMPILER_RT_ABI du_int __fixunsdfdi(double a) {
    if (a <= 0.0) return 0;
    su_int high = a / 4294967296.f;                // a / 0x1p32f;
    su_int low = a - (double)high * 4294967296.f;  // high * 0x1p32f;
    return ((du_int)high << 32) | low;
}

COMPILER_RT_ABI di_int __fixdfdi(double a) {
    if (a < 0.0) {
        return -__fixunsdfdi(-a);
    }
    return __fixunsdfdi(a);
}
