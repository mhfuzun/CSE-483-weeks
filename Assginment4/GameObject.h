#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <stdbool.h>

#include "collision.h"

typedef enum {
    GAME_PRIMITIVE_CUBE = 0,
    GAME_PRIMITIVE_CYLINDER = 1,
    GAME_PRIMITIVE_PLANE = 2,
    GAME_PRIMITIVE_SPHERE = 3
} GamePrimitiveType;

typedef struct GameObject GameObject;

typedef void (*GameObjectOwnerCollisionFn)(void *owner_object,
                                           void *other_owner_object,
                                           GameObject *self_part,
                                           GameObject *other_part);

struct GameObject {
    const char *name;
    GamePrimitiveType primitive_type;

    float local_x;
    float local_y;
    float local_z;

    float local_rot_x;
    float local_rot_y;
    float local_rot_z;

    float world_x;
    float world_y;
    float world_z;

    float size_x;
    float size_y;
    float size_z;
    float radius;

    float color_r;
    float color_g;
    float color_b;

    float collider_offset_x;
    float collider_offset_y;
    float collider_offset_z;

    bool visible;
    Collision *collider;

    void *owner_object;
    GameObjectOwnerCollisionFn owner_collision_callback;
};

GameObject *game_object_create(const char *name, GamePrimitiveType primitive_type);
void game_object_destroy(GameObject *object, CollisionHeap *heap);

void game_object_set_owner(GameObject *object,
                           void *owner_object,
                           GameObjectOwnerCollisionFn owner_collision_callback);

void game_object_set_local_position(GameObject *object, float x, float y, float z);
void game_object_set_local_rotation(GameObject *object, float rx, float ry, float rz);
void game_object_set_color(GameObject *object, float r, float g, float b);
void game_object_set_size(GameObject *object, float sx, float sy, float sz);
void game_object_set_radius(GameObject *object, float radius);
void game_object_set_collider_offset(GameObject *object, float ox, float oy, float oz);

bool game_object_attach_collider(GameObject *object,
                                 CollisionHeap *heap,
                                 CollisionClass class_type,
                                 CollisionShape shape_type);
void game_object_detach_collider(GameObject *object, CollisionHeap *heap);
void game_object_set_collider_box(GameObject *object, float hx, float hy, float hz);
void game_object_set_collider_sphere(GameObject *object, float radius);

void game_object_sync_world(GameObject *object, float owner_x, float owner_y, float owner_z);
void game_object_draw(const GameObject *object);

#endif
