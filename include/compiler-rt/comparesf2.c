#define SINGLE_PRECISION
#include "fp_lib.h"

#include "fp_compare_impl.inc"

COMPILER_RT_ABI CMP_RESULT __lesf2(fp_t a, fp_t b) { return __leXf2__(a, b); }
COMPILER_RT_ALIAS(__lesf2, __eqsf2)
COMPILER_RT_ALIAS(__lesf2, __ltsf2)
COMPILER_RT_ALIAS(__lesf2, __nesf2)

COMPILER_RT_ABI CMP_RESULT __gesf2(fp_t a, fp_t b) { return __geXf2__(a, b); }

COMPILER_RT_ALIAS(__gesf2, __gtsf2)

COMPILER_RT_ABI CMP_RESULT __unordsf2(fp_t a, fp_t b) {
    return __unordXf2__(a, b);
}
