#include "ptime.h"

#include <time.h>

Time getCurrentTime(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    Time t;
    t.ns = (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;

    return t;
}

double getDeltaTime(Time *lastTime) {
    Time currentTime = getCurrentTime();

    uint64_t deltaNs = currentTime.ns - lastTime->ns;

    *lastTime = currentTime;

    return (double)deltaNs / 1000000000.0;
}
