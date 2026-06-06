#include "backend.h"

int init(int width, int height) {
    return 1;
}

void present(uint32_t *pixels, int width, int height) {
    return;
}

void shutdown(void) {
    return;
}

int should_close(void) {
    return 0;
}

backend_t backend = {
    .init = init,
    .present = present,
    .should_close = should_close,
    .shutdown = shutdown
};

backend_t *get_fb0_backend(void) {
    return &backend;
}
