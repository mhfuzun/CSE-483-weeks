#include "vector.h"

vector3_t vector3(float x, float y, float z) {
    vector3_t v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

vector3_t vector3_add(vector3_t *a, vector3_t *b) {
    vector3_t v;
    v.x = a->x + b->x;
    v.y = a->y + b->y;
    v.z = a->z + b->z;
    return v;
}

vector3_t vector3_sub(vector3_t *a, vector3_t *b) {
    vector3_t v;
    v.x = a->x - b->x;
    v.y = a->y - b->y;
    v.z = a->z - b->z;
    return v;
}

vector3_t vector3_mul(vector3_t *a, vector3_t *b) {
    vector3_t v;
    v.x = a->x * b->x;
    v.y = a->y * b->y;
    v.z = a->z * b->z;
    return v;
}

vector4_t vector3to4(vector3_t *a) {
    vector4_t v;
    v.x = a->x;
    v.y = a->y;
    v.z = a->z;
    v.w = 1.0f;
    return v;
}

vector4_t vector4(float x, float y, float z, float w) {
    vector4_t v;
    v.x = x;
    v.y = y;
    v.z = z;
    v.w = w;
    return v;
}

vector4_t matrix4_mul_vector4(const matrix4_t *M, const vector4_t *a) {
    vector4_t v;

    v.x = M->row[0].x * a->x + M->row[0].y * a->y + M->row[0].z * a->z + M->row[0].w * a->w;
    v.y = M->row[1].x * a->x + M->row[1].y * a->y + M->row[1].z * a->z + M->row[1].w * a->w;
    v.z = M->row[2].x * a->x + M->row[2].y * a->y + M->row[2].z * a->z + M->row[2].w * a->w;
    v.w = M->row[3].x * a->x + M->row[3].y * a->y + M->row[3].z * a->z + M->row[3].w * a->w;

    return v;
}

matrix4_t matrix4_mul(const matrix4_t *A, const matrix4_t *B) {
    matrix4_t C;

    for (int k = 0; k < 4; k++) {
        C.row[k].x =
            A->row[k].x * B->row[0].x +
            A->row[k].y * B->row[1].x +
            A->row[k].z * B->row[2].x +
            A->row[k].w * B->row[3].x;

        C.row[k].y =
            A->row[k].x * B->row[0].y +
            A->row[k].y * B->row[1].y +
            A->row[k].z * B->row[2].y +
            A->row[k].w * B->row[3].y;

        C.row[k].z =
            A->row[k].x * B->row[0].z +
            A->row[k].y * B->row[1].z +
            A->row[k].z * B->row[2].z +
            A->row[k].w * B->row[3].z;

        C.row[k].w =
            A->row[k].x * B->row[0].w +
            A->row[k].y * B->row[1].w +
            A->row[k].z * B->row[2].w +
            A->row[k].w * B->row[3].w;
    }

    return C;
}
