#ifndef __BACKEND_H__
#define __BACKEND_H__

#include <stdint.h>

typedef struct {
    int width;
    int height;
    uint32_t *pixels;
} Framebuffer;

typedef struct {
    int  (*init)(int width, int height);
    void (*present)(uint32_t *pixels, int width, int height);
    void (*shutdown)(void);
} DisplayBackend;

#endif // __BACKEND_H__
