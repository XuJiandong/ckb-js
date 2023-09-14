#define QUAD_PRECISION
#include "fp_lib.h"

#include "fp_add_impl.inc"

COMPILER_RT_ABI fp_t __addtf3(fp_t a, fp_t b) { return __addXf3__(a, b); }
