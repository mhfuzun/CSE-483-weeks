
#include "lectures.h"

#define SAFE_BUFFER_SIZE  16384

uint32_t *color_buffer;
float    *depth_buffer;
int width = 800;
int height = 600;

static vertex_t bufferA[SAFE_BUFFER_SIZE];
static renderTriangle_t bufferB[SAFE_BUFFER_SIZE];

static int bufferA_ctr;
static int bufferB_ctr;

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
    matrix4_t M = getModelMatrix(model);
    M = matrix4_mul(cameraProjection, &M);
    
    for (int i = 0; i < vertex_count; i++) {
        bufferA[i].clip_pos = matrix4_mul_vec4(&M, &(v[i].clip_pos));
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

void renderBuffer(void) {
    for (int i=0; i<bufferB_ctr; i++) {
        lineDraw_Bresenham(
            putPixelRender,
            bufferB[i].p1.clip_pos.x,
            bufferB[i].p1.clip_pos.y,
            bufferB[i].p2.clip_pos.x,
            bufferB[i].p2.clip_pos.y
        );
        lineDraw_Bresenham(
            putPixelRender,
            bufferB[i].p2.clip_pos.x,
            bufferB[i].p2.clip_pos.y,
            bufferB[i].p3.clip_pos.x,
            bufferB[i].p3.clip_pos.y
        );
        lineDraw_Bresenham(
            putPixelRender,
            bufferB[i].p3.clip_pos.x,
            bufferB[i].p3.clip_pos.y,
            bufferB[i].p1.clip_pos.x,
            bufferB[i].p1.clip_pos.y
        );
    }
}

uint32_t* getColorBuffer( void ) {
    return color_buffer;
}
