#ifndef PHYSICS_H
#define PHYSICS_H

#include <stdbool.h>

#include "force.h"

typedef struct {
    bool enabled;
    float mass;
    float damping;

    float velocity_x;
    float velocity_y;
    float velocity_z;

    float accumulated_force_x;
    float accumulated_force_y;
    float accumulated_force_z;
} PhysicsBody;

void physics_body_init(PhysicsBody *body, float mass, float damping);
void physics_body_apply_force(PhysicsBody *body, Force force);
void physics_body_integrate(PhysicsBody *body, float dt, float *dx, float *dy, float *dz);
void physics_body_remove_normal_component(PhysicsBody *body, float nx, float ny, float nz);

#endif
