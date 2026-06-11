#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "types/vector.h"

typedef struct {
    vector3_t world_pos;
    vector3_t ambient;
    vector3_t diffuse;
    float ka;
    float kd;
    float ks;
    float ns;
} light_t;

#endif // __OBJECT_H__
