
#include "lectures.h"

#define SAFE_BUFFER_SIZE  128000

uint32_t *color_buffer;
float    *depth_buffer;
int width = 800;
int height = 600;

static vertex_projected_t bufferA[SAFE_BUFFER_SIZE];
static renderTriangle_t bufferB[SAFE_BUFFER_SIZE];

static int bufferA_ctr;
static int bufferB_ctr;

static vector3_t vector4_xyz(vector4_t v) {
    return (vector3_t){v.x, v.y, v.z};
}

static vector3_t transform_normal(const matrix4_t *m, vector3_t n) {
    float a = m->row[0].x;
    float b = m->row[0].y;
    float c = m->row[0].z;
    float d = m->row[1].x;
    float e = m->row[1].y;
    float f = m->row[1].z;
    float g = m->row[2].x;
    float h = m->row[2].y;
    float i = m->row[2].z;

    float det = a * (e * i - f * h)
              - b * (d * i - f * g)
              + c * (d * h - e * g);

    if (fabsf(det) < 0.000001f) {
        return n;
    }

    float inv_det = 1.0f / det;
    vector3_t r = {
        ((e * i - f * h) * n.x + (f * g - d * i) * n.y + (d * h - e * g) * n.z) * inv_det,
        ((c * h - b * i) * n.x + (a * i - c * g) * n.y + (b * g - a * h) * n.z) * inv_det,
        ((b * f - c * e) * n.x + (c * d - a * f) * n.y + (a * e - b * d) * n.z) * inv_det,
    };

    return r;
}

int render_init(
    int _width,
    int _height
) {
    width = _width;
    height = _height;

    color_buffer = malloc(width * height * sizeof(uint32_t));
    depth_buffer = malloc(width * height * sizeof(float));

    if (!color_buffer || !depth_buffer) {
        fprintf(stderr, "Failed to allocate frame buffers.\n");
        free(color_buffer);
        free(depth_buffer);
        color_buffer = NULL;
        depth_buffer = NULL;
        return 0;
    }

    return 1;
}

void render_shutdownn( void ) {
    free(color_buffer);
    free(depth_buffer);
}

void clear_color_buffer(void) {
    for (int i = 0; i < width * height; i++) {
        color_buffer[i] = 0xff111827u;
    }
}

void clear_depth_buffer(void) {
    for (int i = 0; i < width * height; i++) {
        depth_buffer[i] = INFINITY;
    }
}

void clearBuffers(void) {
    bufferA_ctr = 0;
    bufferB_ctr = 0;
}

/**
 * addRenderQueue:
 *  - add mesh(verteces and triangles)
 */
void addRenderQueue(
    vertex_t *v,
    int vertex_count,
    triangle_t *t,
    int triangleCount,
    texture_t* texture,
    transform_t* model,
    matrix4_t* cameraProjection,
    matrix4_t* viewport
) {
    if (vertex_count <= 0 || vertex_count > SAFE_BUFFER_SIZE || triangleCount <= 0) {
        return;
    }

    int startTriangle = bufferB_ctr;

    // Step 1
    // Transform vertices to clip space: Projection * View * Model * position
    matrix4_t MModel = getModelMatrix(model);
    matrix4_t MProject = matrix4_mul(cameraProjection, &MModel);
    
    for (int i = 0; i < vertex_count; i++) {
        bufferA[i].clip_pos = matrix4_mul_vec4(&MProject, &(v[i].clip_pos));
        bufferA[i].uv = v[i].uv;

        vector4_t world_pos = matrix4_mul_vec4(&MModel, &(v[i].clip_pos));
        bufferA[i].world_pos = vector4_xyz(world_pos);

        bufferA[i].normal = transform_normal(&MModel, v[i].normal);
        if (normalizeVector3(&bufferA[i].normal) < 0) {
            bufferA[i].normal = (vector3_t){0.0f, 0.0f, 1.0f};
        }

        bufferA[i].inv_w = 1.0f / bufferA[i].clip_pos.w;
    }

    // Step 2
    // clipping
    for (int i=0; i<triangleCount; i++) {
        if (
            t[i].p1 < 0 || t[i].p1 >= vertex_count ||
            t[i].p2 < 0 || t[i].p2 >= vertex_count ||
            t[i].p3 < 0 || t[i].p3 >= vertex_count
        ) {
            continue;
        }

        renderTriangle_t clipped[16];
        int produced = clipPolygon(
            clipped,
            0,
            &(bufferA[t[i].p1]),
            &(bufferA[t[i].p2]),
            &(bufferA[t[i].p3])
        );

        if (bufferB_ctr + produced > SAFE_BUFFER_SIZE) {
            break;
        }

        for (int j = 0; j < produced; j++) {
            clipped[j].texture = t[i].texture ? t[i].texture : texture;
            bufferB[bufferB_ctr++] = clipped[j];
        }
    }

    // Step 3
    // make homogenous
    for (int i=startTriangle; i<bufferB_ctr; i++) {
        makeHomogenousVector4(&(bufferB[i].p1.clip_pos));
        makeHomogenousVector4(&(bufferB[i].p2.clip_pos));
        makeHomogenousVector4(&(bufferB[i].p3.clip_pos));
    }

    // Step 4
    // Viewport Transformation
    for (int i = startTriangle; i < bufferB_ctr; i++) {
        bufferB[i].p1.clip_pos = matrix4_mul_vec4(viewport, &(bufferB[i].p1.clip_pos));
        bufferB[i].p2.clip_pos = matrix4_mul_vec4(viewport, &(bufferB[i].p2.clip_pos));
        bufferB[i].p3.clip_pos = matrix4_mul_vec4(viewport, &(bufferB[i].p3.clip_pos));
    }
}

void putPixelRender(int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return;
    }

    color_buffer[y * width + x] = 0xffffffffu;
}

void renderBuffer(
    vector3_t* viewer_world_pos,
    light_t* light
) {
    for (int i=0; i<bufferB_ctr; i++) {
        polygon_render( // rasterize
            color_buffer,
            depth_buffer,
            width,
            height,
            &bufferB[i],
            bufferB[i].texture,
            viewer_world_pos,
            light
        );
    }
}

uint32_t* getColorBuffer( void ) {
    return color_buffer;
}
