#ifndef __VECTOR_H__
#define __VECTOR_H__

typedef struct {
    float x;
    float y;
    float z;
} vector3_t;

typedef struct {
    float x;
    float y;
} vector2_t;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} vector4_t;

typedef struct {
    vector4_t row[4];
} matrix4_t;

#endif // __VECTOR_H__
