#ifndef __MESH_H__
#define __MESH_H__

#include <stddef.h>
#include <stdint.h>
#include "types/vector.h"

typedef struct {
    vector4_t clip_pos;
    vector3_t normal;
    vector2_t uv;
} vertex_t;

typedef struct {
    uint32_t* color_buffer;
    int width;
    int height;

    // Eğer texture v=0 altta olacak şekilde okunacaksa 1 yap.
    // Eğer v=0 üstte olacaksa 0 bırak.
    int invert_y;
} texture_t;

typedef struct {
    char* name;
    char* diffuse_texture_path;

    vector3_t ambient;
    vector3_t diffuse;
    vector3_t specular;
    vector3_t emission;

    float shininess;
    float optical_density;
    float dissolve;
    int illum;

    texture_t diffuse_texture;
    int has_diffuse_texture;
} material_t;

typedef struct {
    int p1;
    int p2;
    int p3;
    material_t* material;
    texture_t* texture;
} triangle_t;

typedef struct {
    vector4_t clip_pos;
    vector3_t normal;
    vector2_t uv;

    vector3_t world_pos;
    float inv_w;
} vertex_projected_t;

typedef struct {
    vertex_projected_t p1;
    vertex_projected_t p2;
    vertex_projected_t p3;
    texture_t* texture;
} renderTriangle_t;

typedef struct {
    char* name;
    vertex_t* vertices;
    size_t vertex_count;
    triangle_t* triangles;
    size_t triangle_count;
    material_t* materials;
    size_t material_count;
} mesh_t;

int mesh_load_obj(const char *path, mesh_t *mesh);
mesh_t mesh_create_cube(float size);
void mesh_free(mesh_t *mesh);

#endif // __MESH_H__
