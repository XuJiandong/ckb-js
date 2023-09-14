#define DOUBLE_PRECISION
#include "fp_lib.h"
typedef du_int fixuint_t;
#include "fp_fixuint_impl.inc"

COMPILER_RT_ABI du_int __fixunsdfdi(fp_t a) { return __fixuint(a); }
