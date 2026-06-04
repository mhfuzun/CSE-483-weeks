#ifndef COLLISION_H
#define COLLISION_H

#include <stdbool.h>
#include <stddef.h>

typedef enum {
    COLLISION_CLASS_STATIC = 0,
    COLLISION_CLASS_DYNAMIC = 1
} CollisionClass;

typedef enum {
    COLLISION_SHAPE_AABB = 0,
    COLLISION_SHAPE_CUBE = 1,
    COLLISION_SHAPE_SPHERE = 2
} CollisionShape;

typedef struct {
    float x;
    float y;
    float z;
} CollisionVec3;

struct Collision;
typedef struct Collision Collision;

typedef void (*CollisionCallback)(void *self_object, void *other_object);

typedef struct {
    Collision **items;
    size_t size;
    size_t capacity;
} CollisionHeap;

struct Collision {
    void *owner;
    CollisionClass class_type;
    CollisionShape shape_type;
    CollisionVec3 center;
    CollisionVec3 half_extents;
    float radius;
    bool enabled;
    CollisionCallback on_collision;
};

CollisionHeap *collision_heap_create(size_t initial_capacity);
void collision_heap_destroy(CollisionHeap *heap);
bool collision_heap_push(CollisionHeap *heap, Collision *collider);
bool collision_heap_remove(CollisionHeap *heap, Collision *collider);

Collision *collision_create(void *owner, CollisionClass class_type, CollisionShape shape_type);
void collision_destroy(Collision *collider);

void collision_set_enabled(Collision *collider, bool enabled);
void collision_set_position(Collision *collider, float x, float y, float z);
void collision_set_half_extents(Collision *collider, float hx, float hy, float hz);
void collision_set_radius(Collision *collider, float radius);
void collision_set_callback(Collision *collider, CollisionCallback callback);

void collision_system_call(CollisionHeap *heap);

#endif
