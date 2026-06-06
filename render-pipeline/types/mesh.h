#ifndef __MESH_H__
#define __MESH_H__

#include <stddef.h>
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

typedef struct {
    char* name;
    vertex_t* vertices;
    size_t vertex_count;
    triangle_t* triangles;
    size_t triangle_count;
} mesh_t;

int mesh_load_obj(const char *path, mesh_t *mesh);
mesh_t mesh_create_cube(float size);
void mesh_free(mesh_t *mesh);

#endif // __MESH_H__
