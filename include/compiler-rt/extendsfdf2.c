
#define DOUBLE_PRECISION
#define SRC_SINGLE
#define DST_DOUBLE
#include "fp_extend_impl.inc"

COMPILER_RT_ABI double __extendsfdf2(float a) { return __extendXfYf2__(a); }
