#include "mesh.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <png.h>

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

typedef struct {
    vector2_t *items;
    size_t count;
    size_t capacity;
} vector2_array_t;

typedef struct {
    vector3_t *items;
    size_t count;
    size_t capacity;
} vector3_array_t;

typedef struct {
    material_t *items;
    size_t count;
    size_t capacity;
} material_array_t;

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
    mesh->materials = NULL;
    mesh->material_count = 0;
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

static int reserve_vector2(vector2_array_t *array, size_t needed) {
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

    vector2_t *items = realloc(array->items, new_capacity * sizeof(array->items[0]));
    if (!items) {
        return 0;
    }

    array->items = items;
    array->capacity = new_capacity;
    return 1;
}

static int reserve_vector3(vector3_array_t *array, size_t needed) {
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

    vector3_t *items = realloc(array->items, new_capacity * sizeof(array->items[0]));
    if (!items) {
        return 0;
    }

    array->items = items;
    array->capacity = new_capacity;
    return 1;
}

static int reserve_materials(material_array_t *array, size_t needed) {
    if (needed <= array->capacity) {
        return 1;
    }

    size_t new_capacity = array->capacity ? array->capacity * 2 : 8;
    while (new_capacity < needed) {
        if (new_capacity > SIZE_MAX / 2) {
            return 0;
        }
        new_capacity *= 2;
    }

    material_t *items = realloc(array->items, new_capacity * sizeof(array->items[0]));
    if (!items) {
        return 0;
    }

    array->items = items;
    array->capacity = new_capacity;
    return 1;
}

static void texture_free(texture_t *texture) {
    if (!texture) {
        return;
    }

    free(texture->color_buffer);
    texture->color_buffer = NULL;
    texture->width = 0;
    texture->height = 0;
    texture->invert_y = 0;
}

static void material_free(material_t *material) {
    if (!material) {
        return;
    }

    free(material->name);
    free(material->diffuse_texture_path);
    texture_free(&material->diffuse_texture);
    memset(material, 0, sizeof(*material));
}

static material_t material_default(const char *name) {
    material_t material = {
        .name = mesh_strdup(name),
        .ambient = {0.0f, 0.0f, 0.0f},
        .diffuse = {0.8f, 0.8f, 0.8f},
        .specular = {0.0f, 0.0f, 0.0f},
        .emission = {0.0f, 0.0f, 0.0f},
        .shininess = 0.0f,
        .optical_density = 1.0f,
        .dissolve = 1.0f,
        .illum = 0,
        .diffuse_texture = {0},
        .has_diffuse_texture = 0,
    };

    return material;
}

static int append_vertex(vertex_array_t *array, float x, float y, float z) {
    if (!reserve_vertices(array, array->count + 1)) {
        return 0;
    }

    array->items[array->count++] = (vertex_t){
        .clip_pos = {x, y, z, 1.0f},
        .normal = {0.0f, 0.0f, 1.0f},
        .uv = {0.0f, 0.0f},
    };
    return 1;
}

static int append_vertex_copy(vertex_array_t *array, vertex_t vertex, int *index) {
    if (!reserve_vertices(array, array->count + 1)) {
        return 0;
    }

    if (array->count > (size_t)INT_MAX) {
        return 0;
    }

    *index = (int)array->count;
    array->items[array->count++] = vertex;
    return 1;
}

static int append_texcoord(vector2_array_t *array, float u, float v) {
    if (!reserve_vector2(array, array->count + 1)) {
        return 0;
    }

    array->items[array->count++] = (vector2_t){u, v};
    return 1;
}

static int append_normal(vector3_array_t *array, float x, float y, float z) {
    if (!reserve_vector3(array, array->count + 1)) {
        return 0;
    }

    vector3_t normal = {x, y, z};
    normalizeVector3(&normal);
    array->items[array->count++] = normal;
    return 1;
}

static int append_triangle(
    triangle_array_t *array,
    int p1,
    int p2,
    int p3,
    material_t *material
) {
    if (!reserve_triangles(array, array->count + 1)) {
        return 0;
    }

    texture_t *texture = NULL;
    if (material && material->has_diffuse_texture) {
        texture = &material->diffuse_texture;
    }

    array->items[array->count++] = (triangle_t){
        .p1 = p1,
        .p2 = p2,
        .p3 = p3,
        .material = material,
        .texture = texture,
    };
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

static int file_exists(const char *path) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        return 0;
    }

    fclose(file);
    return 1;
}

static char *path_parent(const char *path) {
    const char *slash = strrchr(path, '/');
    if (!slash) {
        return mesh_strdup(".");
    }

    size_t len = (size_t)(slash - path);
    if (len == 0) {
        len = 1;
    }

    char *parent = malloc(len + 1);
    if (!parent) {
        return NULL;
    }

    memcpy(parent, path, len);
    parent[len] = '\0';
    return parent;
}

static char *path_join(const char *base, const char *relative) {
    if (!relative || relative[0] == '\0') {
        return NULL;
    }

    if (relative[0] == '/') {
        return mesh_strdup(relative);
    }

    size_t base_len = strlen(base);
    size_t rel_len = strlen(relative);
    int needs_slash = base_len > 0 && base[base_len - 1] != '/';

    char *path = malloc(base_len + (size_t)needs_slash + rel_len + 1);
    if (!path) {
        return NULL;
    }

    memcpy(path, base, base_len);
    if (needs_slash) {
        path[base_len++] = '/';
    }
    memcpy(path + base_len, relative, rel_len + 1);
    return path;
}

static char *resolve_asset_path(const char *base_dir, const char *relative) {
    char *path = path_join(base_dir, relative);
    if (!path) {
        return NULL;
    }

    if (file_exists(path)) {
        return path;
    }

    free(path);
    char *texture_dir = path_join(base_dir, "texture");
    if (!texture_dir) {
        return NULL;
    }

    path = path_join(texture_dir, relative);
    free(texture_dir);
    if (path && file_exists(path)) {
        return path;
    }

    free(path);
    return NULL;
}

static int texture_load_png(const char *path, texture_t *texture) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        return 0;
    }

    png_byte signature[8];
    if (fread(signature, 1, sizeof(signature), file) != sizeof(signature) ||
        png_sig_cmp(signature, 0, sizeof(signature)) != 0
    ) {
        fclose(file);
        return 0;
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        fclose(file);
        return 0;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_read_struct(&png, NULL, NULL);
        fclose(file);
        return 0;
    }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_read_struct(&png, &info, NULL);
        fclose(file);
        texture_free(texture);
        return 0;
    }

    png_init_io(png, file);
    png_set_sig_bytes(png, sizeof(signature));
    png_read_info(png, info);

    png_uint_32 png_width = png_get_image_width(png, info);
    png_uint_32 png_height = png_get_image_height(png, info);
    int color_type = png_get_color_type(png, info);
    int bit_depth = png_get_bit_depth(png, info);

    if (bit_depth == 16) {
        png_set_strip_16(png);
    }
    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png);
    }
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
        png_set_expand_gray_1_2_4_to_8(png);
    }
    if (png_get_valid(png, info, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png);
    }
    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
        png_set_gray_to_rgb(png);
    }
    if (!(color_type & PNG_COLOR_MASK_ALPHA)) {
        png_set_filler(png, 0xff, PNG_FILLER_AFTER);
    }

    png_read_update_info(png, info);

    png_size_t rowbytes = png_get_rowbytes(png, info);
    png_bytep pixels = malloc(rowbytes * png_height);
    png_bytep *rows = malloc(sizeof(rows[0]) * png_height);
    uint32_t *color_buffer = malloc(sizeof(color_buffer[0]) * png_width * png_height);

    if (!pixels || !rows || !color_buffer || png_width > (png_uint_32)INT_MAX || png_height > (png_uint_32)INT_MAX) {
        free(pixels);
        free(rows);
        free(color_buffer);
        png_destroy_read_struct(&png, &info, NULL);
        fclose(file);
        return 0;
    }

    for (png_uint_32 y = 0; y < png_height; y++) {
        rows[y] = pixels + y * rowbytes;
    }

    png_read_image(png, rows);

    for (png_uint_32 y = 0; y < png_height; y++) {
        png_bytep row = rows[y];
        for (png_uint_32 x = 0; x < png_width; x++) {
            png_bytep px = row + x * 4;
            color_buffer[y * png_width + x] =
                ((uint32_t)px[3] << 24) |
                ((uint32_t)px[0] << 16) |
                ((uint32_t)px[1] << 8) |
                (uint32_t)px[2];
        }
    }

    texture_free(texture);
    texture->color_buffer = color_buffer;
    texture->width = (int)png_width;
    texture->height = (int)png_height;
    texture->invert_y = 1;

    free(pixels);
    free(rows);
    png_destroy_read_struct(&png, &info, NULL);
    fclose(file);
    return 1;
}

static material_t *append_material(material_array_t *materials, const char *name) {
    if (!reserve_materials(materials, materials->count + 1)) {
        return NULL;
    }

    material_t material = material_default(name);
    if (!material.name) {
        return NULL;
    }

    materials->items[materials->count] = material;
    return &materials->items[materials->count++];
}

static material_t *find_material(material_array_t *materials, const char *name) {
    for (size_t i = 0; i < materials->count; i++) {
        if (materials->items[i].name && strcmp(materials->items[i].name, name) == 0) {
            return &materials->items[i];
        }
    }

    return NULL;
}

static int parse_vector3(char *text, vector3_t *v) {
    return sscanf(text, "%f %f %f", &v->x, &v->y, &v->z) == 3;
}

static char *last_path_token(char *text) {
    char *cursor = skip_space(text);
    trim_right(cursor);

    char *last = cursor;
    char *token = strtok(cursor, " \t\r\n");
    while (token) {
        last = token;
        token = strtok(NULL, " \t\r\n");
    }

    return last && *last ? last : NULL;
}

static int material_set_diffuse_texture(
    material_t *material,
    const char *mtl_dir,
    const char *texture_name
) {
    char *path = resolve_asset_path(mtl_dir, texture_name);
    if (!path) {
        return 1;
    }

    texture_t texture = {0};
    if (!texture_load_png(path, &texture)) {
        free(path);
        return 1;
    }

    free(material->diffuse_texture_path);
    texture_free(&material->diffuse_texture);

    material->diffuse_texture_path = path;
    material->diffuse_texture = texture;
    material->has_diffuse_texture = 1;
    return 1;
}

static int load_mtl_file(const char *path, material_array_t *materials) {
    FILE *file = fopen(path, "r");
    if (!file) {
        return 0;
    }

    char *mtl_dir = path_parent(path);
    if (!mtl_dir) {
        fclose(file);
        return 0;
    }

    char line[4096];
    material_t *current = NULL;
    int ok = 1;

    while (ok && fgets(line, sizeof(line), file)) {
        char *cursor = skip_space(line);

        if (*cursor == '\0' || *cursor == '#') {
            continue;
        }

        if (strncmp(cursor, "newmtl", 6) == 0 && isspace((unsigned char)cursor[6])) {
            char *name = skip_space(cursor + 6);
            trim_right(name);
            current = append_material(materials, name);
            ok = current != NULL;
        } else if (current && cursor[0] == 'K' && cursor[1] == 'a' && isspace((unsigned char)cursor[2])) {
            ok = parse_vector3(cursor + 2, &current->ambient);
        } else if (current && cursor[0] == 'K' && cursor[1] == 'd' && isspace((unsigned char)cursor[2])) {
            ok = parse_vector3(cursor + 2, &current->diffuse);
        } else if (current && cursor[0] == 'K' && cursor[1] == 's' && isspace((unsigned char)cursor[2])) {
            ok = parse_vector3(cursor + 2, &current->specular);
        } else if (current && cursor[0] == 'K' && cursor[1] == 'e' && isspace((unsigned char)cursor[2])) {
            ok = parse_vector3(cursor + 2, &current->emission);
        } else if (current && strncmp(cursor, "Ns", 2) == 0 && isspace((unsigned char)cursor[2])) {
            ok = sscanf(cursor + 2, "%f", &current->shininess) == 1;
        } else if (current && strncmp(cursor, "Ni", 2) == 0 && isspace((unsigned char)cursor[2])) {
            ok = sscanf(cursor + 2, "%f", &current->optical_density) == 1;
        } else if (current && cursor[0] == 'd' && isspace((unsigned char)cursor[1])) {
            ok = sscanf(cursor + 1, "%f", &current->dissolve) == 1;
        } else if (current && strncmp(cursor, "illum", 5) == 0 && isspace((unsigned char)cursor[5])) {
            ok = sscanf(cursor + 5, "%d", &current->illum) == 1;
        } else if (current && strncmp(cursor, "map_Kd", 6) == 0 && isspace((unsigned char)cursor[6])) {
            char *texture_name = last_path_token(cursor + 6);
            ok = texture_name ? material_set_diffuse_texture(current, mtl_dir, texture_name) : 0;
        }
    }

    if (ferror(file)) {
        ok = 0;
    }

    free(mtl_dir);
    fclose(file);
    return ok;
}

static int parse_obj_index(
    const char *token,
    size_t item_count,
    int *index,
    const char **endptr
) {
    char *end;
    errno = 0;
    long obj_index = strtol(token, &end, 10);
    if (errno != 0 || end == token || obj_index == 0) {
        return 0;
    }

    long zero_based = obj_index > 0 ? obj_index - 1 : (long)item_count + obj_index;
    if (zero_based < 0 || (size_t)zero_based >= item_count || zero_based > INT_MAX) {
        return 0;
    }

    *index = (int)zero_based;
    *endptr = end;
    return 1;
}

static int parse_face_vertex(
    const char *token,
    vertex_array_t *vertices,
    size_t position_count,
    const vector2_array_t *texcoords,
    const vector3_array_t *normals,
    int *index
) {
    const char *cursor;
    int position_index;
    if (!parse_obj_index(token, position_count, &position_index, &cursor)) {
        return 0;
    }

    vertex_t vertex = vertices->items[position_index];

    if (*cursor == '/') {
        cursor++;
        if (*cursor != '/' && *cursor != '\0') {
            int texcoord_index;
            if (!parse_obj_index(cursor, texcoords->count, &texcoord_index, &cursor)) {
                return 0;
            }
            vertex.uv = texcoords->items[texcoord_index];
        }

        if (*cursor == '/') {
            int normal_index;
            cursor++;
            if (*cursor != '\0') {
                if (!parse_obj_index(cursor, normals->count, &normal_index, &cursor)) {
                    return 0;
                }
                vertex.normal = normals->items[normal_index];
            }
        }
    }

    if (*cursor != '\0') {
        return 0;
    }

    return append_vertex_copy(vertices, vertex, index);
}

static int parse_face_line(
    char *line,
    vertex_array_t *vertices,
    size_t position_count,
    const vector2_array_t *texcoords,
    const vector3_array_t *normals,
    triangle_array_t *triangles,
    material_t *material
) {
    int face_indices[128];
    int face_count = 0;
    char *token = strtok(line, " \t\r\n");

    while (token) {
        if (face_count >= (int)(sizeof(face_indices) / sizeof(face_indices[0]))) {
            return 0;
        }
        if (!parse_face_vertex(
            token,
            vertices,
            position_count,
            texcoords,
            normals,
            &face_indices[face_count]
        )) {
            return 0;
        }

        face_count++;
        token = strtok(NULL, " \t\r\n");
    }

    if (face_count < 3) {
        return 0;
    }

    for (int i = 1; i < face_count - 1; i++) {
        if (!append_triangle(
            triangles,
            face_indices[0],
            face_indices[i],
            face_indices[i + 1],
            material
        )) {
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

    char *obj_dir = path_parent(path);
    if (!obj_dir) {
        fclose(file);
        return 0;
    }

    vertex_array_t vertices = {0};
    triangle_array_t triangles = {0};
    vector2_array_t texcoords = {0};
    vector3_array_t normals = {0};
    material_array_t materials = {0};
    material_t *current_material = NULL;
    size_t position_count = 0;
    int position_count_locked = 0;
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
        } else if (cursor[0] == 'v' && cursor[1] == 't' && isspace((unsigned char)cursor[2])) {
            float u;
            float v;
            if (sscanf(cursor + 2, "%f %f", &u, &v) < 2) {
                ok = 0;
                break;
            }
            ok = append_texcoord(&texcoords, u, v);
        } else if (cursor[0] == 'v' && cursor[1] == 'n' && isspace((unsigned char)cursor[2])) {
            float x;
            float y;
            float z;
            if (sscanf(cursor + 2, "%f %f %f", &x, &y, &z) != 3) {
                ok = 0;
                break;
            }
            ok = append_normal(&normals, x, y, z);
        } else if (cursor[0] == 'f' && isspace((unsigned char)cursor[1])) {
            if (!position_count_locked) {
                position_count = vertices.count;
                position_count_locked = 1;
            }
            ok = parse_face_line(
                cursor + 1,
                &vertices,
                position_count,
                &texcoords,
                &normals,
                &triangles,
                current_material
            );
        } else if (strncmp(cursor, "mtllib", 6) == 0 && isspace((unsigned char)cursor[6])) {
            char *mtl_name = skip_space(cursor + 6);
            trim_right(mtl_name);
            char *mtl_path = resolve_asset_path(obj_dir, mtl_name);
            ok = mtl_path ? load_mtl_file(mtl_path, &materials) : 0;
            free(mtl_path);
        } else if (strncmp(cursor, "usemtl", 6) == 0 && isspace((unsigned char)cursor[6])) {
            char *name = skip_space(cursor + 6);
            trim_right(name);
            current_material = find_material(&materials, name);
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
        free(texcoords.items);
        free(normals.items);
        for (size_t i = 0; i < materials.count; i++) {
            material_free(&materials.items[i]);
        }
        free(materials.items);
        free(obj_dir);
        mesh_free(mesh);
        return 0;
    }

    if (!mesh->name) {
        mesh->name = mesh_strdup(path);
        if (!mesh->name) {
            free(vertices.items);
            free(triangles.items);
            free(texcoords.items);
            free(normals.items);
            for (size_t i = 0; i < materials.count; i++) {
                material_free(&materials.items[i]);
            }
            free(materials.items);
            free(obj_dir);
            return 0;
        }
    }

    if (position_count_locked && vertices.count > position_count) {
        size_t used_count = vertices.count - position_count;
        memmove(
            vertices.items,
            vertices.items + position_count,
            used_count * sizeof(vertices.items[0])
        );
        vertices.count = used_count;

        for (size_t i = 0; i < triangles.count; i++) {
            triangles.items[i].p1 -= (int)position_count;
            triangles.items[i].p2 -= (int)position_count;
            triangles.items[i].p3 -= (int)position_count;
        }
    }

    mesh->vertices = vertices.items;
    mesh->vertex_count = vertices.count;
    mesh->triangles = triangles.items;
    mesh->triangle_count = triangles.count;
    mesh->materials = materials.items;
    mesh->material_count = materials.count;

    free(texcoords.items);
    free(normals.items);
    free(obj_dir);

    printf("Model [%s] is loaded, vertex count: %ld, triangle count: %ld, material count: %ld\n",
        mesh->name,
        mesh->vertex_count,
        mesh->triangle_count,
        mesh->material_count
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
        {{-h, -h, -h, 1.0f}, {-0.5773503f, -0.5773503f, -0.5773503f}, {0.0f, 0.0f}},
        {{ h, -h, -h, 1.0f}, { 0.5773503f, -0.5773503f, -0.5773503f}, {1.0f, 0.0f}},
        {{ h,  h, -h, 1.0f}, { 0.5773503f,  0.5773503f, -0.5773503f}, {1.0f, 1.0f}},
        {{-h,  h, -h, 1.0f}, {-0.5773503f,  0.5773503f, -0.5773503f}, {0.0f, 1.0f}},
        {{-h, -h,  h, 1.0f}, {-0.5773503f, -0.5773503f,  0.5773503f}, {0.0f, 0.0f}},
        {{ h, -h,  h, 1.0f}, { 0.5773503f, -0.5773503f,  0.5773503f}, {1.0f, 0.0f}},
        {{ h,  h,  h, 1.0f}, { 0.5773503f,  0.5773503f,  0.5773503f}, {1.0f, 1.0f}},
        {{-h,  h,  h, 1.0f}, {-0.5773503f,  0.5773503f,  0.5773503f}, {0.0f, 1.0f}},
    };
    triangle_t triangles[] = {
        {.p1 = 0, .p2 = 1, .p3 = 2},
        {.p1 = 0, .p2 = 2, .p3 = 3},
        {.p1 = 1, .p2 = 5, .p3 = 6},
        {.p1 = 1, .p2 = 6, .p3 = 2},
        {.p1 = 5, .p2 = 4, .p3 = 7},
        {.p1 = 5, .p2 = 7, .p3 = 6},
        {.p1 = 4, .p2 = 0, .p3 = 3},
        {.p1 = 4, .p2 = 3, .p3 = 7},
        {.p1 = 3, .p2 = 2, .p3 = 6},
        {.p1 = 3, .p2 = 6, .p3 = 7},
        {.p1 = 4, .p2 = 5, .p3 = 1},
        {.p1 = 4, .p2 = 1, .p3 = 0},
    };

    memcpy(mesh.vertices, vertices, sizeof(vertices));
    memcpy(mesh.triangles, triangles, sizeof(triangles));
    return mesh;
}

void mesh_free(mesh_t *mesh) {
    if (!mesh) {
        return;
    }

    for (size_t i = 0; i < mesh->material_count; i++) {
        material_free(&mesh->materials[i]);
    }

    free(mesh->name);
    free(mesh->vertices);
    free(mesh->triangles);
    free(mesh->materials);
    mesh_clear(mesh);
}
