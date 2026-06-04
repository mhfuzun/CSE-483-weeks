#include "collision.h"

#include <math.h>
#include <stdlib.h>

static bool is_dynamic_pair(const Collision *a, const Collision *b) {
    return a->class_type == COLLISION_CLASS_DYNAMIC || b->class_type == COLLISION_CLASS_DYNAMIC;
}

static void get_world_aabb(const Collision *c, CollisionVec3 *min_v, CollisionVec3 *max_v) {
    CollisionVec3 ext = c->half_extents;

    if (c->shape_type == COLLISION_SHAPE_SPHERE) {
        ext.x = c->radius;
        ext.y = c->radius;
        ext.z = c->radius;
    }

    min_v->x = c->center.x - ext.x;
    min_v->y = c->center.y - ext.y;
    min_v->z = c->center.z - ext.z;

    max_v->x = c->center.x + ext.x;
    max_v->y = c->center.y + ext.y;
    max_v->z = c->center.z + ext.z;
}

static bool overlap_on_axis(float min_a, float max_a, float min_b, float max_b) {
    return !(max_a < min_b || max_b < min_a);
}

static bool aabb_vs_aabb(const CollisionVec3 *min_a,
                         const CollisionVec3 *max_a,
                         const CollisionVec3 *min_b,
                         const CollisionVec3 *max_b) {
    return overlap_on_axis(min_a->x, max_a->x, min_b->x, max_b->x) &&
           overlap_on_axis(min_a->y, max_a->y, min_b->y, max_b->y) &&
           overlap_on_axis(min_a->z, max_a->z, min_b->z, max_b->z);
}

static float clampf(float value, float min_v, float max_v) {
    if (value < min_v) {
        return min_v;
    }
    if (value > max_v) {
        return max_v;
    }
    return value;
}

static bool sphere_vs_sphere(const Collision *a, const Collision *b) {
    const float dx = a->center.x - b->center.x;
    const float dy = a->center.y - b->center.y;
    const float dz = a->center.z - b->center.z;
    const float radii = a->radius + b->radius;
    const float dist_sq = dx * dx + dy * dy + dz * dz;
    return dist_sq <= radii * radii;
}

static bool sphere_vs_box(const Collision *sphere, const Collision *box) {
    CollisionVec3 min_b = {0.0f, 0.0f, 0.0f};
    CollisionVec3 max_b = {0.0f, 0.0f, 0.0f};
    get_world_aabb(box, &min_b, &max_b);

    const float cx = clampf(sphere->center.x, min_b.x, max_b.x);
    const float cy = clampf(sphere->center.y, min_b.y, max_b.y);
    const float cz = clampf(sphere->center.z, min_b.z, max_b.z);

    const float dx = sphere->center.x - cx;
    const float dy = sphere->center.y - cy;
    const float dz = sphere->center.z - cz;
    const float dist_sq = dx * dx + dy * dy + dz * dz;
    return dist_sq <= sphere->radius * sphere->radius;
}

static bool narrow_phase_check(const Collision *a, const Collision *b) {
    if (a->shape_type == COLLISION_SHAPE_SPHERE && b->shape_type == COLLISION_SHAPE_SPHERE) {
        return sphere_vs_sphere(a, b);
    }

    if (a->shape_type == COLLISION_SHAPE_SPHERE &&
        (b->shape_type == COLLISION_SHAPE_CUBE || b->shape_type == COLLISION_SHAPE_AABB)) {
        return sphere_vs_box(a, b);
    }

    if (b->shape_type == COLLISION_SHAPE_SPHERE &&
        (a->shape_type == COLLISION_SHAPE_CUBE || a->shape_type == COLLISION_SHAPE_AABB)) {
        return sphere_vs_box(b, a);
    }

    CollisionVec3 min_a = {0.0f, 0.0f, 0.0f};
    CollisionVec3 max_a = {0.0f, 0.0f, 0.0f};
    CollisionVec3 min_b = {0.0f, 0.0f, 0.0f};
    CollisionVec3 max_b = {0.0f, 0.0f, 0.0f};
    get_world_aabb(a, &min_a, &max_a);
    get_world_aabb(b, &min_b, &max_b);
    return aabb_vs_aabb(&min_a, &max_a, &min_b, &max_b);
}

CollisionHeap *collision_heap_create(size_t initial_capacity) {
    CollisionHeap *heap = (CollisionHeap *)calloc(1, sizeof(CollisionHeap));
    if (!heap) {
        return NULL;
    }

    if (initial_capacity == 0) {
        initial_capacity = 8;
    }

    heap->items = (Collision **)calloc(initial_capacity, sizeof(Collision *));
    if (!heap->items) {
        free(heap);
        return NULL;
    }

    heap->capacity = initial_capacity;
    heap->size = 0;
    return heap;
}

void collision_heap_destroy(CollisionHeap *heap) {
    size_t i = 0;
    if (!heap) {
        return;
    }

    for (i = 0; i < heap->size; ++i) {
        collision_destroy(heap->items[i]);
    }
    free(heap->items);
    free(heap);
}

static bool collision_heap_reserve(CollisionHeap *heap, size_t new_capacity) {
    Collision **grown = NULL;
    size_t i = 0;
    if (!heap || new_capacity <= heap->capacity) {
        return true;
    }

    grown = (Collision **)calloc(new_capacity, sizeof(Collision *));
    if (!grown) {
        return false;
    }

    for (i = 0; i < heap->size; ++i) {
        grown[i] = heap->items[i];
    }

    free(heap->items);
    heap->items = grown;
    heap->capacity = new_capacity;
    return true;
}

bool collision_heap_push(CollisionHeap *heap, Collision *collider) {
    size_t new_capacity = 0;
    if (!heap || !collider) {
        return false;
    }

    if (heap->size == heap->capacity) {
        new_capacity = heap->capacity * 2;
        if (!collision_heap_reserve(heap, new_capacity)) {
            return false;
        }
    }

    heap->items[heap->size++] = collider;
    return true;
}

bool collision_heap_remove(CollisionHeap *heap, Collision *collider) {
    size_t i = 0;
    if (!heap || !collider) {
        return false;
    }

    for (i = 0; i < heap->size; ++i) {
        if (heap->items[i] == collider) {
            heap->items[i] = heap->items[heap->size - 1];
            heap->items[heap->size - 1] = NULL;
            heap->size--;
            return true;
        }
    }

    return false;
}

Collision *collision_create(void *owner, CollisionClass class_type, CollisionShape shape_type) {
    Collision *collider = (Collision *)calloc(1, sizeof(Collision));
    if (!collider) {
        return NULL;
    }

    collider->owner = owner;
    collider->class_type = class_type;
    collider->shape_type = shape_type;
    collider->half_extents = (CollisionVec3){0.5f, 0.5f, 0.5f};
    collider->radius = 0.5f;
    collider->enabled = true;
    collider->on_collision = NULL;
    return collider;
}

void collision_destroy(Collision *collider) {
    free(collider);
}

void collision_set_enabled(Collision *collider, bool enabled) {
    if (!collider) {
        return;
    }
    collider->enabled = enabled;
}

void collision_set_position(Collision *collider, float x, float y, float z) {
    if (!collider) {
        return;
    }
    collider->center.x = x;
    collider->center.y = y;
    collider->center.z = z;
}

void collision_set_half_extents(Collision *collider, float hx, float hy, float hz) {
    if (!collider) {
        return;
    }
    collider->half_extents.x = fabsf(hx);
    collider->half_extents.y = fabsf(hy);
    collider->half_extents.z = fabsf(hz);
}

void collision_set_radius(Collision *collider, float radius) {
    if (!collider) {
        return;
    }
    collider->radius = fabsf(radius);
}

void collision_set_callback(Collision *collider, CollisionCallback callback) {
    if (!collider) {
        return;
    }
    collider->on_collision = callback;
}

void collision_system_call(CollisionHeap *heap) {
    size_t i = 0;
    size_t j = 0;
    if (!heap) {
        return;
    }

    for (i = 0; i < heap->size; ++i) {
        Collision *a = heap->items[i];
        if (!a || !a->enabled) {
            continue;
        }

        for (j = i + 1; j < heap->size; ++j) {
            Collision *b = heap->items[j];
            CollisionVec3 min_a = {0.0f, 0.0f, 0.0f};
            CollisionVec3 max_a = {0.0f, 0.0f, 0.0f};
            CollisionVec3 min_b = {0.0f, 0.0f, 0.0f};
            CollisionVec3 max_b = {0.0f, 0.0f, 0.0f};

            if (!b || !b->enabled) {
                continue;
            }

            if (!is_dynamic_pair(a, b)) {
                continue;
            }

            get_world_aabb(a, &min_a, &max_a);
            get_world_aabb(b, &min_b, &max_b);

            if (!aabb_vs_aabb(&min_a, &max_a, &min_b, &max_b)) {
                continue;
            }

            if (!narrow_phase_check(a, b)) {
                continue;
            }

            if (a->on_collision) {
                a->on_collision(a->owner, b->owner);
            }
            if (b->on_collision) {
                b->on_collision(b->owner, a->owner);
            }
        }
    }
}
