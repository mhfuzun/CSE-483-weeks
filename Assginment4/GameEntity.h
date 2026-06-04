#ifndef GAMEENTITY_H
#define GAMEENTITY_H

#include <stddef.h>

#include "GameObject.h"
#include "physics/physics.h"

struct GameEntity;
typedef struct GameEntity GameEntity;

typedef void (*GameEntityCollisionFn)(GameEntity *self,
                                      GameEntity *other,
                                      GameObject *self_part,
                                      GameObject *other_part);

struct GameEntity {
    const char *name;

    float x;
    float y;
    float z;

    float prev_x;
    float prev_y;
    float prev_z;

    GameObject **parts;
    size_t part_count;
    size_t part_capacity;

    PhysicsBody physics;
    int physics_enabled;

    int collision_count;
    float accumulated_normal_x;
    float accumulated_normal_y;
    float accumulated_normal_z;

    int is_colliding;

    GameEntityCollisionFn on_collision;
};

GameEntity *game_entity_create(const char *name, size_t initial_part_capacity);
void game_entity_destroy(GameEntity *entity, CollisionHeap *heap);

void game_entity_set_on_collision(GameEntity *entity, GameEntityCollisionFn callback);
void game_entity_set_position(GameEntity *entity, float x, float y, float z);
void game_entity_move(GameEntity *entity, float dx, float dy, float dz);

GameObject *game_entity_add_primitive(GameEntity *entity,
                                      const char *part_name,
                                      GamePrimitiveType primitive_type);

bool game_entity_attach_part_collider(GameEntity *entity,
                                      GameObject *part,
                                      CollisionHeap *heap,
                                      CollisionClass class_type,
                                      CollisionShape shape_type);

void game_entity_sync_parts(GameEntity *entity);
void game_entity_draw(const GameEntity *entity);

void game_entity_enable_physics(GameEntity *entity, float mass, float damping);
void game_entity_apply_force(GameEntity *entity, Force force);
void game_entity_begin_frame(GameEntity *entity);
void game_entity_integrate_physics(GameEntity *entity, float dt);
void game_entity_resolve_collisions(GameEntity *entity);

void game_entity_part_collision_bridge(void *owner_object,
                                       void *other_owner_object,
                                       GameObject *self_part,
                                       GameObject *other_part);

#endif
