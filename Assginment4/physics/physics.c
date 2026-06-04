#include "physics.h"

#include <math.h>
#include <stddef.h>

void physics_body_init(PhysicsBody *body, float mass, float damping) {
    if (!body) {
        return;
    }

    body->enabled = true;
    body->mass = (mass <= 0.0f) ? 1.0f : mass;
    body->damping = (damping < 0.0f) ? 0.0f : damping;

    body->velocity_x = 0.0f;
    body->velocity_y = 0.0f;
    body->velocity_z = 0.0f;

    body->accumulated_force_x = 0.0f;
    body->accumulated_force_y = 0.0f;
    body->accumulated_force_z = 0.0f;
}

void physics_body_apply_force(PhysicsBody *body, Force force) {
    if (!body || !body->enabled) {
        return;
    }

    body->accumulated_force_x += force.x;
    body->accumulated_force_y += force.y;
    body->accumulated_force_z += force.z;

    (void)force.duration;
}

void physics_body_integrate(PhysicsBody *body, float dt, float *dx, float *dy, float *dz) {
    float inv_mass = 0.0f;
    float damping_factor = 0.0f;

    if (dx) {
        *dx = 0.0f;
    }
    if (dy) {
        *dy = 0.0f;
    }
    if (dz) {
        *dz = 0.0f;
    }

    if (!body || !body->enabled || dt <= 0.0f) {
        return;
    }

    inv_mass = 1.0f / body->mass;

    body->velocity_x += body->accumulated_force_x * inv_mass * dt;
    body->velocity_y += body->accumulated_force_y * inv_mass * dt;
    body->velocity_z += body->accumulated_force_z * inv_mass * dt;

    body->accumulated_force_x = 0.0f;
    body->accumulated_force_y = 0.0f;
    body->accumulated_force_z = 0.0f;

    damping_factor = 1.0f / (1.0f + body->damping * dt);
    body->velocity_x *= damping_factor;
    body->velocity_y *= damping_factor;
    body->velocity_z *= damping_factor;

    if (fabsf(body->velocity_x) < 0.0001f) {
        body->velocity_x = 0.0f;
    }
    if (fabsf(body->velocity_y) < 0.0001f) {
        body->velocity_y = 0.0f;
    }
    if (fabsf(body->velocity_z) < 0.0001f) {
        body->velocity_z = 0.0f;
    }

    if (dx) {
        *dx = body->velocity_x * dt;
    }
    if (dy) {
        *dy = body->velocity_y * dt;
    }
    if (dz) {
        *dz = body->velocity_z * dt;
    }
}

void physics_body_remove_normal_component(PhysicsBody *body, float nx, float ny, float nz) {
    float len_sq = 0.0f;
    float dot = 0.0f;

    if (!body || !body->enabled) {
        return;
    }

    len_sq = nx * nx + ny * ny + nz * nz;
    if (len_sq < 0.000001f) {
        return;
    }

    dot = body->velocity_x * nx + body->velocity_y * ny + body->velocity_z * nz;

    body->velocity_x -= nx * dot;
    body->velocity_y -= ny * dot;
    body->velocity_z -= nz * dot;
}
