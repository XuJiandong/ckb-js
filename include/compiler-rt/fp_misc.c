#define DOUBLE_PRECISION

#include "fp_lib.h"

int isnan(double x) { return __builtin_isnan(x); }

int isfinite(double x) { return __builtin_isfinite(x); }

double trunc(double x) { return __builtin_truncl(x); }
