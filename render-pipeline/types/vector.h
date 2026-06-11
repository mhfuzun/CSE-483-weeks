#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <math.h>

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


static inline int normalizeVector3(vector3_t *v) {
    float len2 =
        v->x * v->x +
        v->y * v->y +
        v->z * v->z;

    if (len2 == 0.0f) {
        return -1;
    }

    float inv_len = 1.0f / sqrtf(len2);

    v->x *= inv_len;
    v->y *= inv_len;
    v->z *= inv_len;

    return 1;
}

static inline vector3_t unit_vector_between_two_point(
    const vector3_t *v1,
    const vector3_t *v2
) {
    vector3_t n;

    n.x = v2->x - v1->x;
    n.y = v2->y - v1->y;
    n.z = v2->z - v1->z;

    normalizeVector3(&n);

    return n;
}

static inline float dot_product_vector3(
    const vector3_t *v1,
    const vector3_t *v2
) {
    return (
        v1->x * v2->x +
        v1->y * v2->y +
        v1->z * v2->z
    );
}

static inline vector3_t add_vector3(
    const vector3_t *v1,
    const vector3_t *v2
) {
    return (vector3_t) {
        .x = v1->x + v2->x,
        .y = v1->y + v2->y,
        .z = v1->z + v2->z,
    };
}

static inline vector3_t scale_vector3(
    const vector3_t *v,
    float s
) {
    return (vector3_t) {
        .x = v->x * s,
        .y = v->y * s,
        .z = v->z * s
    };
}

static inline vector3_t multiply_vector3(
    const vector3_t *v1,
    const vector3_t *v2
) {
    return (vector3_t) {
        .x = v1->x * v2->x,
        .y = v1->y * v2->y,
        .z = v1->z * v2->z
    };
}

static inline float clamp01(float x) {
    if (x < 0.0f) return 0.0f;
    if (x > 1.0f) return 1.0f;
    return x;
}

#endif // __VECTOR_H__
