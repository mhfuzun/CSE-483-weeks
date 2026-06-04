#include "house.h"

#include <stddef.h>

static int setup_wall(GameEntity *house,
                      CollisionHeap *heap,
                      const char *name,

                      float lx,
                      float ly,
                      float lz,

                      float rx,
                      float ry,
                      float rz,

                      float width,
                      float height,

                      float hx,
                      float hy,
                      float hz,
                    
                      float color_r,
                      float color_g,
                      float color_b) {
    GameObject *wall = game_entity_add_primitive(house, name, GAME_PRIMITIVE_PLANE);
    if (!wall) {
        return 0;
    }

    game_object_set_local_position(wall, lx, ly, lz);
    game_object_set_local_rotation(wall, rx, ry, rz);
    game_object_set_size(wall, width, 0.0f, height);
    game_object_set_color(wall, color_r, color_g, color_b);

    if (!game_entity_attach_part_collider(
            house,
            wall,
            heap,
            COLLISION_CLASS_STATIC,
            COLLISION_SHAPE_CUBE)) {
        return 0;
    }

    game_object_set_collider_box(wall, hx, hy, hz);
    return 1;
}

GameEntity *house_create(CollisionHeap *heap,
                         float x,
                         float y,
                         float z,
                         GameEntityCollisionFn on_collision) {
    GameEntity *house = game_entity_create("House", 8);
    GameObject *floor = NULL;

    if (!house || !heap) {
        game_entity_destroy(house, heap);
        return NULL;
    }

    game_entity_set_on_collision(house, on_collision);
    game_entity_set_position(house, x, y, z);

    if (!setup_wall(house, heap, "HouseFrontWallLeftPart", 
            -0.9375f, 1.0f, 1.5f, 
            90.0f, 0.0f, 0.0f, 
            1.125f, 2.0f, 
            1.5f, 1.0f, 0.06f,
            0.8f, 0.55f, 0.35f)) {
        game_entity_destroy(house, heap);
        return NULL;
    }

    if (!setup_wall(house, heap, "HouseFrontWallRightPart", 
            0.9375f, 1.0f, 1.5f, 
            90.0f, 0.0f, 0.0f, 
            1.125f, 2.0f, 
            1.5f, 1.0f, 0.06f,
            0.8f, 0.55f, 0.35f)) {
        game_entity_destroy(house, heap);
        return NULL;
    }

    if (!setup_wall(house, heap, "HouseFrontWallUpOfDoor", 
            0.0f, 1.85f, 1.5f, 
            90.0f, 0.0f, 0.0f, 
            0.75f, 0.30f, 
            1.5f, 1.0f, 0.06f,
            0.0f, 0.55f, 0.35f)) {
        game_entity_destroy(house, heap);
        return NULL;
    }

    if (!setup_wall(house, heap, "HouseBackWall", 
            0.0f, 1.0f, -1.5f, 
            90.0f, 0.0f, 0.0f, 
            3.0f, 2.0f, 
            1.5f, 1.0f, 0.06f,
            0.8f, 0.55f, 0.35f)) {
        game_entity_destroy(house, heap);
        return NULL;
    }

    if (!setup_wall(house, heap, "HouseLeftWall", 
            -1.5f, 1.0f, 0.0f, 
            90.0f, 0.0f, 90.0f, 
            3.0f, 2.0f, 
            0.06f, 1.0f, 1.5f,
            0.8f, 0.55f, 0.35f)) {
        game_entity_destroy(house, heap);
        return NULL;
    }

    if (!setup_wall(house, heap, "HouseRightWall", 
            1.5f, 1.0f, 0.0f, 
            90.0f, 0.0f, 90.0f, 
            3.0f, 2.0f, 
            0.06f, 1.0f, 1.5f,
            0.8f, 0.55f, 0.35f)) {
        game_entity_destroy(house, heap);
        return NULL;
    }

    floor = game_entity_add_primitive(house, "HouseFloor", GAME_PRIMITIVE_PLANE);
    if (!floor) {
        game_entity_destroy(house, heap);
        return NULL;
    }

    game_object_set_local_position(floor, 0.0f, 0.01f, 0.0f);
    game_object_set_size(floor, 3.0f, 0.0f, 3.0f);
    game_object_set_color(floor, 0.5f, 0.35f, 0.2f);

    if (!game_entity_attach_part_collider(
            house,
            floor,
            heap,
            COLLISION_CLASS_STATIC,
            COLLISION_SHAPE_CUBE)) {
        game_entity_destroy(house, heap);
        return NULL;
    }

    game_object_set_collider_box(floor, 1.5f, 0.05f, 1.5f);

    game_entity_sync_parts(house);
    return house;
}
