#ifndef __PTIME_H__
#define __PTIME_H__

#include <stdint.h>

typedef struct {
    uint64_t ns;
} Time;

Time getCurrentTime(void);
double getDeltaTime(Time *lastTime);

#endif // __PTIME_H__