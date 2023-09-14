#define DOUBLE_PRECISION
#include "fp_lib.h"
typedef su_int fixuint_t;
#include "fp_fixuint_impl.inc"

COMPILER_RT_ABI su_int __fixunsdfsi(fp_t a) { return __fixuint(a); }
