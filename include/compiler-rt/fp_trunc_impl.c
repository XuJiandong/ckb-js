#define DOUBLE_PRECISION

#define SRC_DOUBLE
#define DST_SINGLE
#include "fp_trunc_impl.inc"

COMPILER_RT_ABI float __truncdfsf2(double a) { return __truncXfYf2__(a); }
