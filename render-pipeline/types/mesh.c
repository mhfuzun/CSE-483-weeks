#include "mesh.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    vertex_t *items;
    size_t count;
    size_t capacity;
} vertex_array_t;

typedef struct {
    triangle_t *items;
    size_t count;
    size_t capacity;
} triangle_array_t;

static char *mesh_strdup(const char *text) {
    size_t len = strlen(text);
    char *copy = malloc(len + 1);
    if (!copy) {
        return NULL;
    }

    memcpy(copy, text, len + 1);
    return copy;
}

static void mesh_clear(mesh_t *mesh) {
    if (!mesh) {
        return;
    }

    mesh->name = NULL;
    mesh->vertices = NULL;
    mesh->vertex_count = 0;
    mesh->triangles = NULL;
    mesh->triangle_count = 0;
}

static int reserve_vertices(vertex_array_t *array, size_t needed) {
    if (needed <= array->capacity) {
        return 1;
    }

    size_t new_capacity = array->capacity ? array->capacity * 2 : 64;
    while (new_capacity < needed) {
        if (new_capacity > SIZE_MAX / 2) {
            return 0;
        }
        new_capacity *= 2;
    }

    vertex_t *items = realloc(array->items, new_capacity * sizeof(array->items[0]));
    if (!items) {
        return 0;
    }

    array->items = items;
    array->capacity = new_capacity;
    return 1;
}

static int reserve_triangles(triangle_array_t *array, size_t needed) {
    if (needed <= array->capacity) {
        return 1;
    }

    size_t new_capacity = array->capacity ? array->capacity * 2 : 64;
    while (new_capacity < needed) {
        if (new_capacity > SIZE_MAX / 2) {
            return 0;
        }
        new_capacity *= 2;
    }

    triangle_t *items = realloc(array->items, new_capacity * sizeof(array->items[0]));
    if (!items) {
        return 0;
    }

    array->items = items;
    array->capacity = new_capacity;
    return 1;
}

static int append_vertex(vertex_array_t *array, float x, float y, float z) {
    if (!reserve_vertices(array, array->count + 1)) {
        return 0;
    }

    array->items[array->count++] = (vertex_t){{x, y, z, 1.0f}};
    return 1;
}

static int append_triangle(triangle_array_t *array, int p1, int p2, int p3) {
    if (!reserve_triangles(array, array->count + 1)) {
        return 0;
    }

    array->items[array->count++] = (triangle_t){p1, p2, p3};
    return 1;
}

static char *skip_space(char *text) {
    while (*text && isspace((unsigned char)*text)) {
        text++;
    }
    return text;
}

static void trim_right(char *text) {
    size_t len = strlen(text);
    while (len > 0 && isspace((unsigned char)text[len - 1])) {
        text[--len] = '\0';
    }
}

static int parse_face_vertex(const char *token, size_t vertex_count, int *index) {
    char *end;
    errno = 0;
    long obj_index = strtol(token, &end, 10);
    if (errno != 0 || end == token || obj_index == 0) {
        return 0;
    }

    long zero_based = obj_index > 0 ? obj_index - 1 : (long)vertex_count + obj_index;
    if (zero_based < 0 || (size_t)zero_based >= vertex_count || zero_based > INT_MAX) {
        return 0;
    }

    *index = (int)zero_based;
    return 1;
}

static int parse_face_line(char *line, size_t vertex_count, triangle_array_t *triangles) {
    int face_indices[128];
    int face_count = 0;
    char *token = strtok(line, " \t\r\n");

    while (token) {
        if (face_count >= (int)(sizeof(face_indices) / sizeof(face_indices[0]))) {
            return 0;
        }
        if (!parse_face_vertex(token, vertex_count, &face_indices[face_count])) {
            return 0;
        }

        face_count++;
        token = strtok(NULL, " \t\r\n");
    }

    if (face_count < 3) {
        return 0;
    }

    for (int i = 1; i < face_count - 1; i++) {
        if (!append_triangle(triangles, face_indices[0], face_indices[i], face_indices[i + 1])) {
            return 0;
        }
    }

    return 1;
}

static int set_mesh_name(mesh_t *mesh, const char *name) {
    if (mesh->name) {
        return 1;
    }

    mesh->name = mesh_strdup(name);
    return mesh->name != NULL;
}

int mesh_load_obj(const char *path, mesh_t *mesh) {
    if (!path || !mesh) {
        return 0;
    }

    FILE *file = fopen(path, "r");
    if (!file) {
        return 0;
    }

    mesh_clear(mesh);

    vertex_array_t vertices = {0};
    triangle_array_t triangles = {0};
    char line[4096];
    int ok = 1;

    while (ok && fgets(line, sizeof(line), file)) {
        char *cursor = skip_space(line);

        if (cursor[0] == 'v' && isspace((unsigned char)cursor[1])) {
            float x;
            float y;
            float z;
            if (sscanf(cursor + 1, "%f %f %f", &x, &y, &z) != 3) {
                ok = 0;
                break;
            }
            ok = append_vertex(&vertices, x, y, z);
        } else if (cursor[0] == 'f' && isspace((unsigned char)cursor[1])) {
            ok = parse_face_line(cursor + 1, vertices.count, &triangles);
        } else if ((cursor[0] == 'o' || cursor[0] == 'g') && isspace((unsigned char)cursor[1])) {
            char *name = skip_space(cursor + 1);
            trim_right(name);
            if (*name) {
                ok = set_mesh_name(mesh, name);
            }
        }
    }

    if (ferror(file)) {
        ok = 0;
    }
    fclose(file);

    if (!ok || vertices.count == 0 || triangles.count == 0) {
        free(vertices.items);
        free(triangles.items);
        mesh_free(mesh);
        return 0;
    }

    if (!mesh->name) {
        mesh->name = mesh_strdup(path);
        if (!mesh->name) {
            free(vertices.items);
            free(triangles.items);
            return 0;
        }
    }

    mesh->vertices = vertices.items;
    mesh->vertex_count = vertices.count;
    mesh->triangles = triangles.items;
    mesh->triangle_count = triangles.count;

    printf("Model [%s] is loaded, vertex count: %ld, triangle count: %ld\n",
        mesh->name,
        mesh->vertex_count,
        mesh->triangle_count
    );

    return 1;
}

mesh_t mesh_create_cube(float size) {
    mesh_t mesh;
    mesh_clear(&mesh);

    float h = size * 0.5f;
    mesh.name = mesh_strdup("Cube");
    mesh.vertex_count = 8;
    mesh.triangle_count = 12;
    mesh.vertices = malloc(mesh.vertex_count * sizeof(mesh.vertices[0]));
    mesh.triangles = malloc(mesh.triangle_count * sizeof(mesh.triangles[0]));

    if (!mesh.name || !mesh.vertices || !mesh.triangles) {
        mesh_free(&mesh);
        return mesh;
    }

    vertex_t vertices[] = {
        {{-h, -h, -h, 1.0f}},
        {{ h, -h, -h, 1.0f}},
        {{ h,  h, -h, 1.0f}},
        {{-h,  h, -h, 1.0f}},
        {{-h, -h,  h, 1.0f}},
        {{ h, -h,  h, 1.0f}},
        {{ h,  h,  h, 1.0f}},
        {{-h,  h,  h, 1.0f}},
    };
    triangle_t triangles[] = {
        {0, 1, 2}, {0, 2, 3},
        {1, 5, 6}, {1, 6, 2},
        {5, 4, 7}, {5, 7, 6},
        {4, 0, 3}, {4, 3, 7},
        {3, 2, 6}, {3, 6, 7},
        {4, 5, 1}, {4, 1, 0},
    };

    memcpy(mesh.vertices, vertices, sizeof(vertices));
    memcpy(mesh.triangles, triangles, sizeof(triangles));
    return mesh;
}

void mesh_free(mesh_t *mesh) {
    if (!mesh) {
        return;
    }

    free(mesh->name);
    free(mesh->vertices);
    free(mesh->triangles);
    mesh_clear(mesh);
}
