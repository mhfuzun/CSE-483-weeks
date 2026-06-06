#ifndef __VECTOR_H__
#define __VECTOR_H__

typedef struct {
    float x;
    float y;
} vector2_t;

typedef struct {
    float x;
    float y;
    float z;
} vector3_t;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} vector4_t;

typedef struct {
    vector4_t row[4];
} matrix4_t;

static inline void makeHomogenousVector4(vector4_t *v) {
    float inv_w = 1.0f / v->w;
    v->x *= inv_w;
    v->y *= inv_w;
    v->z *= inv_w;
    v->w = 1;
}

#endif // __VECTOR_H__
