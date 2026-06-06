// Lecture 3&4

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "vector.h"
#include "transform.h"

matrix4_t copyMatrix(
    const matrix4_t *A
) {
    return (matrix4_t){
        .row = {
            {.x = A->row[0].x, .y = A->row[0].y, .z = A->row[0].z, .w = A->row[0].w},
            {.x = A->row[1].x, .y = A->row[1].y, .z = A->row[1].z, .w = A->row[1].w},
            {.x = A->row[2].x, .y = A->row[2].y, .z = A->row[2].z, .w = A->row[2].w},
            {.x = A->row[3].x, .y = A->row[3].y, .z = A->row[3].z, .w = A->row[3].w}
        }
    };
}

matrix4_t loadIdentityMatrix(
    void
) {
    return (matrix4_t){
        .row = {
            {.x = 1.0f, .y = 0.0f, .z = 0.0f, .w = 0.0f},
            {.x = 0.0f, .y = 1.0f, .z = 0.0f, .w = 0.0f},
            {.x = 0.0f, .y = 0.0f, .z = 1.0f, .w = 0.0f},
            {.x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f}
        }
    };
}

matrix4_t matrix4_mul(
    const matrix4_t *A,
    const matrix4_t *B
) {
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

vector4_t matrix4_mul_vec4(
    const matrix4_t *M,
    const vector4_t *v
) {
    vector4_t r;

    r.x =
        M->row[0].x * v->x +
        M->row[0].y * v->y +
        M->row[0].z * v->z +
        M->row[0].w * v->w;

    r.y =
        M->row[1].x * v->x +
        M->row[1].y * v->y +
        M->row[1].z * v->z +
        M->row[1].w * v->w;

    r.z =
        M->row[2].x * v->x +
        M->row[2].y * v->y +
        M->row[2].z * v->z +
        M->row[2].w * v->w;

    r.w =
        M->row[3].x * v->x +
        M->row[3].y * v->y +
        M->row[3].z * v->z +
        M->row[3].w * v->w;

    return r;
}

matrix4_t getTranslationMatrix(
    float dx,
    float dy,
    float dz
) {
    return (matrix4_t){
        .row = {
            {.x = 1.0f, .y = 0.0f, .z = 0.0f, .w = dx},
            {.x = 0.0f, .y = 1.0f, .z = 0.0f, .w = dy},
            {.x = 0.0f, .y = 0.0f, .z = 1.0f, .w = dz},
            {.x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f}
        }
    };
}

matrix4_t getRotationMatrix_Zaxis(
    float dTheta
) {
    float cosdT = cos(dTheta);
    float sindT = sin(dTheta);

    return (matrix4_t){
        .row = {
            {.x = cosdT, .y = -sindT, .z = 0.0f, .w = 0.0f},
            {.x = sindT, .y = cosdT, .z = 0.0f, .w = 0.0f},
            {.x = 0.0f, .y = 0.0f, .z = 1.0f, .w = 0.0f},
            {.x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f}
        }
    };
}

matrix4_t getRotationMatrix_Xaxis(
    float dTheta
) {
    float cosdT = cos(dTheta);
    float sindT = sin(dTheta);

    return (matrix4_t){
        .row = {
            {.x = 1.0f, .y = 0.0f, .z = 0.0f, .w = 0.0f},
            {.x = 0.0f, .y = cosdT, .z = -sindT, .w = 0.0f},
            {.x = 0.0f, .y = sindT, .z = cosdT, .w = 0.0f},
            {.x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f}
        }
    };
}

matrix4_t getRotationMatrix_Yaxis(
    float dTheta
) {
    float cosdT = cos(dTheta);
    float sindT = sin(dTheta);

    return (matrix4_t){
        .row = {
            {.x = cosdT, .y = 0.0f, .z = sindT, .w = 0.0f},
            {.x = 0.0f, .y = 1.0f, .z = 0.0f, .w = 0.0f},
            {.x = -sindT, .y = 0.0f, .z = cosdT, .w = 0.0f},
            {.x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f}
        }
    };
}

matrix4_t getScaleMatrix(
    float sx,
    float sy,
    float sz
) {
    return (matrix4_t){
        .row = {
            {.x = sx, .y = 0.0f, .z = 0.0f, .w = 0.0f},
            {.x = 0.0f, .y = sy, .z = 0.0f, .w = 0.0f},
            {.x = 0.0f, .y = 0.0f, .z = sz, .w = 0.0f},
            {.x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f}
        }
    };
}

matrix4_t getShearMatrix(
    float shx,
    float shy,
    float zref
) {
    return (matrix4_t){
        .row = {
            {.x = 1.0f, .y = 0.0f, .z = shx, .w = -shx*zref},
            {.x = 0.0f, .y = 1.0f, .z = shy, .w = -shy*zref},
            {.x = 0.0f, .y = 0.0f, .z = 1.0f, .w = 0.0f},
            {.x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f}
        }
    };
}

matrix4_t matrixTranslate(
    const matrix4_t *A,
    float dx,
    float dy,
    float dz
) {
    matrix4_t M = getTranslationMatrix(dx, dy, dz);
    return matrix4_mul(&M, A);
}

matrix4_t matrixRotateOrigin(
    const matrix4_t *A,
    float dx,
    float dy,
    float dz
) {
    matrix4_t M = copyMatrix(A);
    if (dx != 0) {
        matrix4_t R = getRotationMatrix_Xaxis(dx);
        M = matrix4_mul(&R, &M);
    }
    if (dy != 0) {
        matrix4_t R = getRotationMatrix_Yaxis(dy);
        M = matrix4_mul(&R, &M);
    }
    if (dz != 0) {
        matrix4_t R = getRotationMatrix_Zaxis(dz);
        M = matrix4_mul(&R, &M);
    }
    return M;
}

matrix4_t matrixRotateOriginInverse(
    const matrix4_t *A,
    float rx,
    float ry,
    float rz
) {
    matrix4_t M = copyMatrix(A);

    // Dikkat: inverse için ters sıra + negatif açılar

    if (rz != 0) {
        matrix4_t R = getRotationMatrix_Zaxis(-rz);
        M = matrix4_mul(&R, &M);
    }

    if (ry != 0) {
        matrix4_t R = getRotationMatrix_Yaxis(-ry);
        M = matrix4_mul(&R, &M);
    }

    if (rx != 0) {
        matrix4_t R = getRotationMatrix_Xaxis(-rx);
        M = matrix4_mul(&R, &M);
    }

    return M;
}

matrix4_t matrixScaleOrigin(
    const matrix4_t *A,
    float sx,
    float sy,
    float sz
) {
    matrix4_t S = getScaleMatrix(sx, sy, sz);
    return matrix4_mul(&S, A);
}

matrix4_t matrixShearOrigin(
    const matrix4_t *A,
    float shx,
    float shy,
    float zref
) {
    matrix4_t Sh = getShearMatrix(shx, shy, zref);
    return matrix4_mul(&Sh, A);
}

matrix4_t matrixRotate(
    const matrix4_t *A,
    float ox,
    float oy,
    float oz,
    float dx,
    float dy,
    float dz
) {
    matrix4_t M = copyMatrix(A);
    M = matrixTranslate(&M, -ox, -oy, -oz);
    M = matrixRotateOrigin(&M, dx, dy, dz);
    M = matrixTranslate(&M, ox, oy, oz);
    return M;
}

matrix4_t matrixScale(
    const matrix4_t *A,
    float ox,
    float oy,
    float oz,
    float sx,
    float sy,
    float sz
) {
    matrix4_t M = copyMatrix(A);
    M = matrixTranslate(&M, -ox, -oy, -oz);
    M = matrixScaleOrigin(&M, sx, sy, sz);
    M = matrixTranslate(&M, ox, oy, oz);
    return M;
}

matrix4_t matrixShear(
    const matrix4_t *A,
    float ox,
    float oy,
    float oz,
    float shx,
    float shy,
    float zref
) {
    matrix4_t M = copyMatrix(A);
    M = matrixTranslate(&M, -ox, -oy, -oz);
    M = matrixShearOrigin(&M, shx, shy, zref);
    M = matrixTranslate(&M, ox, oy, oz);
    return M;
}

// model coordinate -> world coordinate
matrix4_t getModelMatrix(
    transform_t* transform
) {
    matrix4_t M = loadIdentityMatrix();

    M = matrixScaleOrigin(
        &M,

        transform->scale.x,
        transform->scale.y,
        transform->scale.z
    );

    M = matrixRotateOrigin(
        &M,
        
        transform->rotation.x,
        transform->rotation.y,
        transform->rotation.z
    );

    M = matrixTranslate(
        &M,
        
        transform->position.x,
        transform->position.y,
        transform->position.z
    );

    return M;
}
