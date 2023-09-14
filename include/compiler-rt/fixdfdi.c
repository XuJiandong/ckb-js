#define DOUBLE_PRECISION
#include "fp_lib.h"

typedef di_int fixint_t;
typedef du_int fixuint_t;
#include "fp_fixint_impl.inc"

COMPILER_RT_ABI di_int __fixdfdi(fp_t a) { return __fixint(a); }
