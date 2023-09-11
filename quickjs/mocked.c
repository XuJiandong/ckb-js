

#include "mocked.h"
int fesetround(int _round) { return 0; }

int fegetround() { return 0; }

// TODO: add panic code to following functions
struct tm *localtime_r(const time_t *a, struct tm *b) { return 0; }
int gettimeofday(struct timeval *restrict tv, struct timezone *restrict tz) {
    return 0;
}
