#ifndef __VERTEX_H__
#define __VERTEX_H__

#include "types/vector.h"

typedef struct {
    vector3_t position;
    vector2_t tex_coord;
    vector3_t normal;
} vertex_t;


#endif // __VERTEX_H__