#include "GameEntity.h"

#include <math.h>
#include <stddef.h>
#include <stdlib.h>

static bool game_entity_reserve_parts(GameEntity *entity, size_t new_capacity) {
    GameObject **grown = NULL;
    size_t i = 0;

    if (!entity || new_capacity <= entity->part_capacity) {
        return true;
    }

    grown = (GameObject **)calloc(new_capacity, sizeof(GameObject *));
    if (!grown) {
        return false;
    }

    for (i = 0; i < entity->part_count; ++i) {
        grown[i] = entity->parts[i];
    }

    free(entity->parts);
    entity->parts = grown;
    entity->part_capacity = new_capacity;
    return true;
}

GameEntity *game_entity_create(const char *name, size_t initial_part_capacity) {
    GameEntity *entity = NULL;

    if (initial_part_capacity == 0) {
        initial_part_capacity = 4;
    }

    entity = (GameEntity *)calloc(1, sizeof(GameEntity));
    if (!entity) {
        return NULL;
    }

    entity->parts = (GameObject **)calloc(initial_part_capacity, sizeof(GameObject *));
    if (!entity->parts) {
        free(entity);
        return NULL;
    }

    entity->name = name;
    entity->part_capacity = initial_part_capacity;
    entity->part_count = 0;
    entity->physics_enabled = 0;
    return entity;
}

void game_entity_destroy(GameEntity *entity, CollisionHeap *heap) {
    size_t i = 0;

    if (!entity) {
        return;
    }

    for (i = 0; i < entity->part_count; ++i) {
        game_object_destroy(entity->parts[i], heap);
    }

    free(entity->parts);
    free(entity);
}

void game_entity_set_on_collision(GameEntity *entity, GameEntityCollisionFn callback) {
    if (!entity) {
        return;
    }

    entity->on_collision = callback;
}

void game_entity_set_position(GameEntity *entity, float x, float y, float z) {
    if (!entity) {
        return;
    }

    entity->x = x;
    entity->y = y;
    entity->z = z;

    game_entity_sync_parts(entity);
}

void game_entity_move(GameEntity *entity, float dx, float dy, float dz) {
    if (!entity) {
        return;
    }

    entity->x += dx;
    entity->y += dy;
    entity->z += dz;

    game_entity_sync_parts(entity);
}

GameObject *game_entity_add_primitive(GameEntity *entity,
                                      const char *part_name,
                                      GamePrimitiveType primitive_type) {
    GameObject *part = NULL;
    size_t new_capacity = 0;

    if (!entity) {
        return NULL;
    }

    if (entity->part_count == entity->part_capacity) {
        new_capacity = entity->part_capacity * 2;
        if (!game_entity_reserve_parts(entity, new_capacity)) {
            return NULL;
        }
    }

    part = game_object_create(part_name, primitive_type);
    if (!part) {
        return NULL;
    }

    game_object_set_owner(part, entity, game_entity_part_collision_bridge);
    entity->parts[entity->part_count++] = part;

    game_object_sync_world(part, entity->x, entity->y, entity->z);
    return part;
}

bool game_entity_attach_part_collider(GameEntity *entity,
                                      GameObject *part,
                                      CollisionHeap *heap,
                                      CollisionClass class_type,
                                      CollisionShape shape_type) {
    if (!entity || !part || !heap) {
        return false;
    }

    if (part->owner_object != entity) {
        game_object_set_owner(part, entity, game_entity_part_collision_bridge);
    }

    if (!game_object_attach_collider(part, heap, class_type, shape_type)) {
        return false;
    }

    game_object_sync_world(part, entity->x, entity->y, entity->z);
    return true;
}

void game_entity_sync_parts(GameEntity *entity) {
    size_t i = 0;

    if (!entity) {
        return;
    }

    for (i = 0; i < entity->part_count; ++i) {
        game_object_sync_world(entity->parts[i], entity->x, entity->y, entity->z);
    }
}

void game_entity_draw(const GameEntity *entity) {
    size_t i = 0;

    if (!entity) {
        return;
    }

    for (i = 0; i < entity->part_count; ++i) {
        game_object_draw(entity->parts[i]);
    }
}

void game_entity_enable_physics(GameEntity *entity, float mass, float damping) {
    if (!entity) {
        return;
    }

    physics_body_init(&entity->physics, mass, damping);
    entity->physics_enabled = 1;
}

void game_entity_apply_force(GameEntity *entity, Force force) {
    if (!entity || !entity->physics_enabled) {
        return;
    }

    physics_body_apply_force(&entity->physics, force);
}

void game_entity_begin_frame(GameEntity *entity) {
    if (!entity) {
        return;
    }

    entity->prev_x = entity->x;
    entity->prev_y = entity->y;
    entity->prev_z = entity->z;

    entity->collision_count = 0;
    entity->accumulated_normal_x = 0.0f;
    entity->accumulated_normal_y = 0.0f;
    entity->accumulated_normal_z = 0.0f;
    entity->is_colliding = 0;
}

void game_entity_integrate_physics(GameEntity *entity, float dt) {
    float dx = 0.0f;
    float dy = 0.0f;
    float dz = 0.0f;

    if (!entity || !entity->physics_enabled) {
        return;
    }

    physics_body_integrate(&entity->physics, dt, &dx, &dy, &dz);
    game_entity_move(entity, dx, dy, dz);
}

void game_entity_resolve_collisions(GameEntity *entity) {
    float nx = 0.0f;
    float ny = 0.0f;
    float nz = 0.0f;
    float normal_len = 0.0f;
    float move_x = 0.0f;
    float move_y = 0.0f;
    float move_z = 0.0f;
    float move_dot = 0.0f;

    if (!entity || !entity->physics_enabled || entity->collision_count == 0) {
        return;
    }

    nx = entity->accumulated_normal_x / (float)entity->collision_count;
    ny = entity->accumulated_normal_y / (float)entity->collision_count;
    nz = entity->accumulated_normal_z / (float)entity->collision_count;

    normal_len = sqrtf(nx * nx + ny * ny + nz * nz);
    if (normal_len < 0.00001f) {
        return;
    }

    nx /= normal_len;
    ny /= normal_len;
    nz /= normal_len;

    physics_body_remove_normal_component(&entity->physics, nx, ny, nz);

    move_x = entity->x - entity->prev_x;
    move_y = entity->y - entity->prev_y;
    move_z = entity->z - entity->prev_z;

    move_dot = move_x * nx + move_y * ny + move_z * nz;

    if (move_dot < 0.0f) {
        entity->x -= nx * move_dot;
        entity->y -= ny * move_dot;
        entity->z -= nz * move_dot;
        game_entity_sync_parts(entity);
    }
}

void game_entity_part_collision_bridge(void *owner_object,
                                       void *other_owner_object,
                                       GameObject *self_part,
                                       GameObject *other_part) {
    GameEntity *self = (GameEntity *)owner_object;
    GameEntity *other = (GameEntity *)other_owner_object;
    float nx = 0.0f;
    float ny = 0.0f;
    float nz = 0.0f;
    float len = 0.0f;

    if (!self || !other || !self_part || !other_part) {
        return;
    }

    if (self == other) {
        return;
    }

    nx = self_part->world_x - other_part->world_x;
    ny = self_part->world_y - other_part->world_y;
    nz = self_part->world_z - other_part->world_z;

    len = sqrtf(nx * nx + ny * ny + nz * nz);
    if (len > 0.00001f) {
        nx /= len;
        ny /= len;
        nz /= len;
    } else {
        nx = 0.0f;
        ny = 1.0f;
        nz = 0.0f;
    }

    self->collision_count += 1;
    self->is_colliding = 1;
    self->accumulated_normal_x += nx;
    self->accumulated_normal_y += ny;
    self->accumulated_normal_z += nz;

    if (self->on_collision) {
        self->on_collision(self, other, self_part, other_part);
    }
}
