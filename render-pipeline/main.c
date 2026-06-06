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

static mesh_t cube;
static mesh_t teapot;

static float rotationSpeed = 1.0f;
static transform_t model;

static void center_mesh_origin(mesh_t *mesh) {
    if (!mesh || !mesh->vertices || mesh->vertex_count == 0) {
        return;
    }

    float min_x = mesh->vertices[0].clip_pos.x;
    float max_x = mesh->vertices[0].clip_pos.x;
    float min_y = mesh->vertices[0].clip_pos.y;
    float max_y = mesh->vertices[0].clip_pos.y;
    float min_z = mesh->vertices[0].clip_pos.z;
    float max_z = mesh->vertices[0].clip_pos.z;

    for (size_t i = 1; i < mesh->vertex_count; i++) {
        vector4_t p = mesh->vertices[i].clip_pos;

        if (p.x < min_x) min_x = p.x;
        if (p.x > max_x) max_x = p.x;
        if (p.y < min_y) min_y = p.y;
        if (p.y > max_y) max_y = p.y;
        if (p.z < min_z) min_z = p.z;
        if (p.z > max_z) max_z = p.z;
    }

    vector3_t center = {
        (min_x + max_x) * 0.5f,
        (min_y + max_y) * 0.5f,
        (min_z + max_z) * 0.5f,
    };

    for (size_t i = 0; i < mesh->vertex_count; i++) {
        mesh->vertices[i].clip_pos.x -= center.x;
        mesh->vertices[i].clip_pos.y -= center.y;
        mesh->vertices[i].clip_pos.z -= center.z;
    }
}

void update_scene(float deltaTime) {
    static float angle;

    angle += deltaTime * rotationSpeed;

    model = (transform_t){
        .position = {0.0f, 0.0f, -5.0f},
        .rotation = {angle, angle * 0.5f, angle * 0.3f},
        .scale = {1.0f, 1.0f, 1.0f},
    };
}

void render_scene(void) {
    transform_t camera = {
        .position = {0.0f, 0.0f, 0.0f},
        .rotation = {0.0f, 0.0f, 0.0f},
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
        teapot.vertices,
        (int)teapot.vertex_count,
        teapot.triangles,
        (int)teapot.triangle_count,
        &model,
        &cameraProjection,
        &viewport
    );
    renderBuffer();
}

void setup_scene( void ) {
    cube = mesh_create_cube(2.0f);
    if (mesh_load_obj("resources/utah_teapot.obj", &teapot)) {
        center_mesh_origin(&teapot);
    }
    // mesh_load_obj("resources/utah_teapot_high.obj", &teapot);
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

    setup_scene();

    while (run) {
        double delta_time = getDeltaTime(&lastTime);
        (void)delta_time;

        clear_color_buffer();
        clear_depth_buffer();

        update_scene(delta_time);
        render_scene();

        backend->present(getColorBuffer(), WINDOW_SIZE_WIDTH, WINDOW_SIZE_HEIGHT);
        run = !backend->should_close();
    }

    backend->shutdown();
    render_shutdownn();

    return 0;
}
