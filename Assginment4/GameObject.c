#include "GameObject.h"

#include <math.h>
#include <stddef.h>
#include <stdlib.h>

#include "objects/object_primitives.h"

static float clamp01(float value) {
    if (value < 0.0f) {
        return 0.0f;
    }
    if (value > 1.0f) {
        return 1.0f;
    }
    return value;
}

static void game_object_collision_bridge(void *self_object, void *other_object) {
    GameObject *self = (GameObject *)self_object;
    GameObject *other = (GameObject *)other_object;

    if (!self || !self->owner_collision_callback || !self->owner_object) {
        return;
    }

    self->owner_collision_callback(
        self->owner_object,
        other ? other->owner_object : NULL,
        self,
        other);
}

GameObject *game_object_create(const char *name, GamePrimitiveType primitive_type) {
    GameObject *object = (GameObject *)calloc(1, sizeof(GameObject));
    if (!object) {
        return NULL;
    }

    object->name = name;
    object->primitive_type = primitive_type;
    object->visible = true;

    object->size_x = 1.0f;
    object->size_y = 1.0f;
    object->size_z = 1.0f;
    object->radius = 0.5f;

    object->color_r = 1.0f;
    object->color_g = 1.0f;
    object->color_b = 1.0f;

    return object;
}

void game_object_destroy(GameObject *object, CollisionHeap *heap) {
    if (!object) {
        return;
    }

    game_object_detach_collider(object, heap);
    free(object);
}

void game_object_set_owner(GameObject *object,
                           void *owner_object,
                           GameObjectOwnerCollisionFn owner_collision_callback) {
    if (!object) {
        return;
    }

    object->owner_object = owner_object;
    object->owner_collision_callback = owner_collision_callback;
}

void game_object_set_local_position(GameObject *object, float x, float y, float z) {
    if (!object) {
        return;
    }

    object->local_x = x;
    object->local_y = y;
    object->local_z = z;
}

void game_object_set_local_rotation(GameObject *object, float rx, float ry, float rz) {
    if (!object) {
        return;
    }

    object->local_rot_x = rx;
    object->local_rot_y = ry;
    object->local_rot_z = rz;
}

void game_object_set_color(GameObject *object, float r, float g, float b) {
    if (!object) {
        return;
    }

    object->color_r = clamp01(r);
    object->color_g = clamp01(g);
    object->color_b = clamp01(b);
}

void game_object_set_size(GameObject *object, float sx, float sy, float sz) {
    if (!object) {
        return;
    }

    object->size_x = fabsf(sx);
    object->size_y = fabsf(sy);
    object->size_z = fabsf(sz);
}

void game_object_set_radius(GameObject *object, float radius) {
    if (!object) {
        return;
    }

    object->radius = fabsf(radius);
}

void game_object_set_collider_offset(GameObject *object, float ox, float oy, float oz) {
    if (!object) {
        return;
    }

    object->collider_offset_x = ox;
    object->collider_offset_y = oy;
    object->collider_offset_z = oz;
}

bool game_object_attach_collider(GameObject *object,
                                 CollisionHeap *heap,
                                 CollisionClass class_type,
                                 CollisionShape shape_type) {
    if (!object || !heap) {
        return false;
    }

    if (object->collider) {
        return true;
    }

    object->collider = collision_create(object, class_type, shape_type);
    if (!object->collider) {
        return false;
    }

    collision_set_callback(object->collider, game_object_collision_bridge);

    if (!collision_heap_push(heap, object->collider)) {
        collision_destroy(object->collider);
        object->collider = NULL;
        return false;
    }

    return true;
}

void game_object_detach_collider(GameObject *object, CollisionHeap *heap) {
    if (!object || !object->collider) {
        return;
    }

    if (heap) {
        collision_heap_remove(heap, object->collider);
    }

    collision_destroy(object->collider);
    object->collider = NULL;
}

void game_object_set_collider_box(GameObject *object, float hx, float hy, float hz) {
    if (!object || !object->collider) {
        return;
    }

    collision_set_half_extents(object->collider, hx, hy, hz);
}

void game_object_set_collider_sphere(GameObject *object, float radius) {
    if (!object || !object->collider) {
        return;
    }

    collision_set_radius(object->collider, radius);
}

void game_object_sync_world(GameObject *object, float owner_x, float owner_y, float owner_z) {
    if (!object) {
        return;
    }

    object->world_x = owner_x + object->local_x;
    object->world_y = owner_y + object->local_y;
    object->world_z = owner_z + object->local_z;

    if (!object->collider) {
        return;
    }

    collision_set_position(
        object->collider,
        object->world_x + object->collider_offset_x,
        object->world_y + object->collider_offset_y,
        object->world_z + object->collider_offset_z);
}

void game_object_draw(const GameObject *object) {
    if (!object || !object->visible) {
        return;
    }

    glColor3f(object->color_r, object->color_g, object->color_b);

    glPushMatrix();
    glTranslatef(object->world_x, object->world_y, object->world_z);
    glRotatef(object->local_rot_x, 1.0f, 0.0f, 0.0f);
    glRotatef(object->local_rot_y, 0.0f, 1.0f, 0.0f);
    glRotatef(object->local_rot_z, 0.0f, 0.0f, 1.0f);

    switch (object->primitive_type) {
        case GAME_PRIMITIVE_CUBE:
            draw_box(object->size_x, object->size_y, object->size_z);
            break;
        case GAME_PRIMITIVE_CYLINDER:
            draw_cylinder(object->radius, object->size_y);
            break;
        case GAME_PRIMITIVE_PLANE:
            draw_plane(object->size_x, object->size_z);
            break;
        case GAME_PRIMITIVE_SPHERE:
            draw_sphere(object->radius);
            break;
        default:
            break;
    }

    glPopMatrix();
}
