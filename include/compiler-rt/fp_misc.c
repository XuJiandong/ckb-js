#define DOUBLE_PRECISION
#include "fp_lib.h"

int isnan(double x) { return __builtin_isnan(x); }

int isfinite(double x) { return __builtin_isfinite(x); }

double trunc(double x) { return __builtin_truncl(x); }

// IEEE-754 default rounding (to nearest, ties to even).
CRT_FE_ROUND_MODE __fe_getround(void) { return CRT_FE_TONEAREST; }

int __fe_raise_inexact(void) { return 0; }
