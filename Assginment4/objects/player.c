#include "player.h"

GameEntity *player_create(CollisionHeap *heap,
                          float x,
                          float y,
                          float z,
                          GameEntityCollisionFn on_collision) {
    GameEntity *player = game_entity_create("Player", 2);
    GameObject *body = NULL;

    if (!player || !heap) {
        game_entity_destroy(player, heap);
        return NULL;
    }

    game_entity_set_on_collision(player, on_collision);
    game_entity_set_position(player, x, y, z);

    body = game_entity_add_primitive(player, "PlayerBody", GAME_PRIMITIVE_SPHERE);
    if (!body) {
        game_entity_destroy(player, heap);
        return NULL;
    }

    game_object_set_local_position(body, 0.0f, 0.28f, 0.0f);
    game_object_set_radius(body, 0.28f);
    game_object_set_color(body, 0.95f, 0.1f, 0.1f);

    if (!game_entity_attach_part_collider(player,
                                          body,
                                          heap,
                                          COLLISION_CLASS_DYNAMIC,
                                          COLLISION_SHAPE_SPHERE)) {
        game_entity_destroy(player, heap);
        return NULL;
    }

    game_object_set_collider_sphere(body, 0.28f);

    game_entity_enable_physics(player, 1.0f, 8.0f);
    game_entity_sync_parts(player);
    return player;
}
