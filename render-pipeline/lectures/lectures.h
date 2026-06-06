#ifndef __LECTURES_H__
#define __LECTURES_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#include "vector.h"
#include "mesh.h"
#include "transform.h"

// Lecture 1
// 1_483lec1line24s
// Line Drawing
typedef void (*putPixel)(int x, int y);

void lineDraw_LineEquation(putPixel pp, int x0, int y0, int x1, int y1);
void lineDraw_DDA(putPixel pp, int x0, int y0, int x1, int y1);
void lineDraw_Bresenham(putPixel pp, int x0, int y0, int x1, int y1);

// Circle Drawing
void circleDraw_PythagoreanTheorem(putPixel pp, int xc, int yc, int r);
void circleDraw_PolarCoordinates(putPixel pp, int xc, int yc, int r);
void circleDraw_Midpoint(putPixel pp, int xc, int yc, int r);

// Lecture 4
// 4_483lec4trans3D26s
matrix4_t loadIdentityMatrix(void);
matrix4_t matrixTranslate(const matrix4_t *A,float dx,float dy,float dz);
matrix4_t matrixRotateOrigin(const matrix4_t *A,float dx,float dy,float dz);
matrix4_t matrixRotateOriginInverse(const matrix4_t *A,float dx,float dy,float dz);
matrix4_t matrixScaleOrigin(const matrix4_t *A,float sx,float sy,float sz);
matrix4_t matrixShearOrigin(const matrix4_t *A,float shx,float shy,float zref);
matrix4_t matrixRotate(const matrix4_t *A,
    float ox,float oy,float oz,
    float dx,float dy,float dz);
matrix4_t matrixScale(const matrix4_t *A,
    float ox,float oy,float oz,
    float sx,float sy,float sz);
matrix4_t matrixShear( const matrix4_t *A,
    float ox, float oy, float oz,
    float shx, float shy, float zref);
vector4_t matrix4_mul_vec4(const matrix4_t *M, const vector4_t *v);
matrix4_t matrix4_mul(const matrix4_t *A, const matrix4_t *B);
matrix4_t getModelMatrix(transform_t* transform);

// Lecture 5
// 5_483lec6b3Dviewing21s
matrix4_t getProjectionMatrix(float n,float f,float projectionAngle,float aspectRatio);
matrix4_t createCameraProjectionMatrix(transform_t camera,float znear,float zfar,float projecitonAngle,float aspectRatio);
matrix4_t createViewPortMatrix(float xmin,float ymin,float xmax,float ymax);
int clipPolygon(renderTriangle_t* triangles, int base, vertex_t* v1, vertex_t* v2, vertex_t* v3);

// Render
int render_init(int _width,int _height);
void render_shutdownn( void );
void clear_color_buffer(void);
void clear_depth_buffer(void);
void clearBuffers(void);
void addRenderQueue(
    vertex_t *v,
    int vertex_count,
    triangle_t *t,
    int triangleCount,
    transform_t* model,
    matrix4_t* cameraProjection,
    matrix4_t* viewport
);
void renderBuffer(void);
uint32_t* getColorBuffer( void );

#endif // __LECTURES_H__
