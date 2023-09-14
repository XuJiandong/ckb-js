
#define DOUBLE_PRECISION
#include "fp_lib.h"

#include "fp_compare_impl.inc"
COMPILER_RT_ABI CMP_RESULT __ledf2(fp_t a, fp_t b) { return __leXf2__(a, b); }
COMPILER_RT_ALIAS(__ledf2, __eqdf2)
COMPILER_RT_ALIAS(__ledf2, __ltdf2)
COMPILER_RT_ALIAS(__ledf2, __nedf2)

COMPILER_RT_ABI CMP_RESULT __gedf2(fp_t a, fp_t b) { return __geXf2__(a, b); }

COMPILER_RT_ALIAS(__gedf2, __gtdf2)

COMPILER_RT_ABI CMP_RESULT __unorddf2(fp_t a, fp_t b) {
    return __unordXf2__(a, b);
}
