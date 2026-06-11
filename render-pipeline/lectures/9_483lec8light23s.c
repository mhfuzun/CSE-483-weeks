#include "lectures.h"

static inline vector3_t light_calculation(
    vector3_t* pixel_normal,
    vector3_t* pixel_world_pos,
    vector3_t* viewer_world_pos,
    light_t* light
) {
    vector3_t L = unit_vector_between_two_point(pixel_world_pos, &light->world_pos);
    vector3_t V = unit_vector_between_two_point(pixel_world_pos, viewer_world_pos);
    vector3_t H = add_vector3(&L, &V);
    normalizeVector3(&H);

    float NL = fmaxf(0.0f, dot_product_vector3(pixel_normal, &L));
    float NH = fmaxf(0.0f, dot_product_vector3(pixel_normal, &H));
    float diffuse_strength = light->kd * NL;
    float specular_strength = NL > 0.0f ? light->ks * powf(NH, light->ns) : 0.0f;

    vector3_t ambient = scale_vector3(&light->ambient, light->ka);
    vector3_t diffuse = scale_vector3(
        &light->diffuse,
        diffuse_strength + specular_strength
    );

    return add_vector3(&ambient, &diffuse);
}

static inline vector3_t texture_sample(
    const texture_t* texture,
    const vector2_t* uv
) {
    if (
        texture == NULL ||
        texture->color_buffer == NULL ||
        texture->width <= 0 ||
        texture->height <= 0
    ) {
        return (vector3_t){1.0f, 0.0f, 1.0f};
    }

    float u = clamp01(uv->x);
    float v = clamp01(uv->y);

    if (texture->invert_y) {
        v = 1.0f - v;
    }

    int x = (int)(u * (float)(texture->width - 1));
    int y = (int)(v * (float)(texture->height - 1));
    uint32_t color = texture->color_buffer[y * texture->width + x];

    return color_to_rgb(color);
}

static inline float edge_function(
    const vector4_t* a,
    const vector4_t* b,
    const vector4_t* p
) {
    return (p->x - a->x) * (b->y - a->y)
         - (p->y - a->y) * (b->x - a->x);
}

static inline void barycentric_calc(
    float inv_area,
    const vector4_t* A,
    const vector4_t* B,
    const vector4_t* C,
    const vector4_t* P,
    float* b0,
    float* b1,
    float* b2
) {
    *b0 = edge_function(B, C, P) * inv_area;
    *b1 = edge_function(C, A, P) * inv_area;
    *b2 = edge_function(A, B, P) * inv_area;
}

static inline vector3_t barycentric_interpolate_vector3_with(
    const vector3_t* v0,
    const vector3_t* v1,
    const vector3_t* v2,
    float b0,
    float b1,
    float b2,
    float pixel_inv_w
) {
    float inv = 1.0f / pixel_inv_w;

    return (vector3_t) {
        .x = (v0->x * b0 + v1->x * b1 + v2->x * b2) * inv,
        .y = (v0->y * b0 + v1->y * b1 + v2->y * b2) * inv,
        .z = (v0->z * b0 + v1->z * b1 + v2->z * b2) * inv,
    };
}

static inline vector2_t barycentric_interpolate_vector2_with(
    const vector2_t* v0,
    const vector2_t* v1,
    const vector2_t* v2,
    float b0,
    float b1,
    float b2,
    float pixel_inv_w
) {
    float inv = 1.0f / pixel_inv_w;

    return (vector2_t) {
        .x = (v0->x * b0 + v1->x * b1 + v2->x * b2) * inv,
        .y = (v0->y * b0 + v1->y * b1 + v2->y * b2) * inv,
    };
}

void polygon_render(
    uint32_t* color_buffer,
    float* depth_buffer,
    int screen_width,
    int screen_height,
    renderTriangle_t* triangle,
    texture_t* texture,
    vector3_t* viewer_world_pos,
    light_t* light
) {
    vertex_projected_t* v0 = &triangle->p1;
    vertex_projected_t* v1 = &triangle->p2;
    vertex_projected_t* v2 = &triangle->p3;

    float area = edge_function(&v0->clip_pos, &v1->clip_pos, &v2->clip_pos);
    if (area == 0.0f) {
        return;
    }

    int min_x = (int)floorf(fminf(v0->clip_pos.x, fminf(v1->clip_pos.x, v2->clip_pos.x)));
    int max_x = (int)ceilf (fmaxf(v0->clip_pos.x, fmaxf(v1->clip_pos.x, v2->clip_pos.x)));
    int min_y = (int)floorf(fminf(v0->clip_pos.y, fminf(v1->clip_pos.y, v2->clip_pos.y)));
    int max_y = (int)ceilf (fmaxf(v0->clip_pos.y, fmaxf(v1->clip_pos.y, v2->clip_pos.y)));

    min_x = min_x < 0 ? 0 : min_x;
    min_y = min_y < 0 ? 0 : min_y;
    max_x = max_x >= screen_width ? screen_width - 1 : max_x;
    max_y = max_y >= screen_height ? screen_height - 1 : max_y;

    float inv_area = 1.0f / area;

    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            vector4_t P = {
                .x = (float)x + 0.5f,
                .y = (float)y + 0.5f,
                .z = 0.0f,
                .w = 1.0f,
            };

            float b0;
            float b1;
            float b2;
            barycentric_calc(
                inv_area,
                &v0->clip_pos,
                &v1->clip_pos,
                &v2->clip_pos,
                &P,
                &b0,
                &b1,
                &b2
            );

            if (b0 < -0.00001f || b1 < -0.00001f || b2 < -0.00001f) {
                continue;
            }

            float pixel_depth =
                v0->clip_pos.z * b0 +
                v1->clip_pos.z * b1 +
                v2->clip_pos.z * b2;

            int index = y * screen_width + x;
            if (pixel_depth >= depth_buffer[index]) {
                continue;
            }

            float pixel_inv_w =
                b0 * v0->inv_w +
                b1 * v1->inv_w +
                b2 * v2->inv_w;

            if (fabsf(pixel_inv_w) < 0.000001f) {
                continue;
            }

            float p0 = b0 * v0->inv_w;
            float p1 = b1 * v1->inv_w;
            float p2 = b2 * v2->inv_w;

            vector3_t pixel_world_pos = barycentric_interpolate_vector3_with(
                &v0->world_pos,
                &v1->world_pos,
                &v2->world_pos,
                p0,
                p1,
                p2,
                pixel_inv_w
            );

            vector3_t pixel_normal = barycentric_interpolate_vector3_with(
                &v0->normal,
                &v1->normal,
                &v2->normal,
                p0,
                p1,
                p2,
                pixel_inv_w
            );
            normalizeVector3(&pixel_normal);

            vector2_t pixel_uv = barycentric_interpolate_vector2_with(
                &v0->uv,
                &v1->uv,
                &v2->uv,
                p0,
                p1,
                p2,
                pixel_inv_w
            );

            vector3_t tex_color = texture_sample(texture, &pixel_uv);
            vector3_t lighting = light_calculation(
                &pixel_normal,
                &pixel_world_pos,
                viewer_world_pos,
                light
            );
            vector3_t shaded = multiply_vector3(&tex_color, &lighting);

            depth_buffer[index] = pixel_depth;
            color_buffer[index] = rgb_to_color(shaded);
        }
    }
}
