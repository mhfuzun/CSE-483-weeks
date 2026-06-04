#include "force.h"

Force force_create(float x, float y, float z, float duration) {
    Force force = {x, y, z, duration};
    return force;
}
