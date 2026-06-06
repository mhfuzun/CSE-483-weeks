#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>

#include "backend.h"
#include "ptime.h"
#include "lectures/lectures.h"

#define WINDOW_SIZE_WIDTH   800
#define WINDOW_SIZE_HEIGHT  600

void render_scene(void) {
    static vertex_t cube_vertices[] = {
        {{-1.0f, -1.0f, -1.0f, 1.0f}},
        {{ 1.0f, -1.0f, -1.0f, 1.0f}},
        {{ 1.0f,  1.0f, -1.0f, 1.0f}},
        {{-1.0f,  1.0f, -1.0f, 1.0f}},
        {{-1.0f, -1.0f,  1.0f, 1.0f}},
        {{ 1.0f, -1.0f,  1.0f, 1.0f}},
        {{ 1.0f,  1.0f,  1.0f, 1.0f}},
        {{-1.0f,  1.0f,  1.0f, 1.0f}},
    };

    static triangle_t cube_triangles[] = {
        {0, 1, 2}, {0, 2, 3},
        {1, 5, 6}, {1, 6, 2},
        {5, 4, 7}, {5, 7, 6},
        {4, 0, 3}, {4, 3, 7},
        {3, 2, 6}, {3, 6, 7},
        {4, 5, 1}, {4, 1, 0},
    };

    static float angle;
    angle += 0.01f;

    transform_t camera = {
        .position = {0.0f, 0.0f, 0.0f},
        .rotation = {0.0f, 0.0f, 0.0f},
        .scale = {1.0f, 1.0f, 1.0f},
    };
    transform_t model = {
        .position = {0.0f, 0.0f, -5.0f},
        .rotation = {angle, angle * 0.7f, angle * 0.3f},
        .scale = {1.0f, 1.0f, 1.0f},
    };

    matrix4_t cameraProjection = createCameraProjectionMatrix(
        camera,
        0.1f,
        100.0f,
        70.0f * 3.1415926535f / 180.0f,
        (float)WINDOW_SIZE_WIDTH / (float)WINDOW_SIZE_HEIGHT
    );
    matrix4_t viewport = createViewPortMatrix(
        0.0f,
        (float)WINDOW_SIZE_HEIGHT,
        (float)WINDOW_SIZE_WIDTH,
        0.0f
    );

    clearBuffers();
    addRenderQueue(
        cube_vertices,
        sizeof(cube_vertices) / sizeof(cube_vertices[0]),
        cube_triangles,
        sizeof(cube_triangles) / sizeof(cube_triangles[0]),
        &model,
        &cameraProjection,
        &viewport
    );
    renderBuffer();
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    bool run = true;
    Time lastTime = getCurrentTime();

    backend_t *backend = get_sdl_backend();
    if (!backend->init(WINDOW_SIZE_WIDTH, WINDOW_SIZE_HEIGHT)) {
        fprintf(stderr, "Failed to initialize display backend.\n");
        return 1;
    }
    if (!render_init(WINDOW_SIZE_WIDTH, WINDOW_SIZE_HEIGHT)) {
        fprintf(stderr, "Failed to initialize render buffers.\n");
        backend->shutdown();
        return 1;
    }

    while (run) {
        double delta_time = getDeltaTime(&lastTime);
        (void)delta_time;

        clear_color_buffer();
        clear_depth_buffer();

        // update_scene(delta_time);
        render_scene();

        backend->present(getColorBuffer(), WINDOW_SIZE_WIDTH, WINDOW_SIZE_HEIGHT);
        run = !backend->should_close();
    }

    backend->shutdown();
    render_shutdownn();

    return 0;
}
