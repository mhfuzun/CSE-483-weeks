#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include "types/vector.h"

typedef struct {
    vector3_t position;
    vector3_t rotation;
    vector3_t scale;
} transform_t;


#endif // __TRANSFORM_H__
