
#define QUAD_PRECISION
#include "fp_lib.h"

#include "fp_mul_impl.inc"

COMPILER_RT_ABI fp_t __multf3(fp_t a, fp_t b) { return __mulXf3__(a, b); }
