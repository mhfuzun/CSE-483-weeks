#ifndef __VERTEX_H__
#define __VERTEX_H__

#include "types/vector.h"

typedef struct {
    vector4_t clip_pos;
} vertex_t;

typedef struct {
    int p1;
    int p2;
    int p3;
} triangle_t;

typedef struct {
    vertex_t p1;
    vertex_t p2;
    vertex_t p3;
} renderTriangle_t;

#endif // __VERTEX_H__
