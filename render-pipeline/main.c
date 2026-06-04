#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "backend.h"
#include "ptime.h"

uint32_t *color_buffer;
float    *depth_buffer;
int width = 800;
int height = 600;

int main(int argc, char **argv) {
    int run = true;
    Time lastTime = getCurrentTime();

    backend_t *backend = get_sdl_backend();
    backend->init(width, height);

    color_buffer = malloc(width * height * sizeof(uint32_t));
    depth_buffer = malloc(width * height * sizeof(float));

    while (run) {
        double delta_time = getDeltaTime(&lastTime);

        clear_color_buffer();
        clear_depth_buffer();

        update_scene(delta_time);
        render_scene();

        backend->present(colorBuffer, width, height);
    }

    backend->shutdown();

    free(color_buffer);
    free(depth_buffer);

    return 0;
}