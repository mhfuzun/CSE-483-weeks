#include "tree.h"

GameEntity *tree_create(CollisionHeap *heap,
                        float x,
                        float y,
                        float z,
                        GameEntityCollisionFn on_collision) {
    GameEntity *tree = game_entity_create("Tree", 4);
    GameObject *trunk = NULL;
    GameObject *head = NULL;

    if (!tree || !heap) {
        game_entity_destroy(tree, heap);
        return NULL;
    }

    game_entity_set_on_collision(tree, on_collision);
    game_entity_set_position(tree, x, y, z);

    trunk = game_entity_add_primitive(tree, "TreeTrunk", GAME_PRIMITIVE_CYLINDER);
    if (!trunk) {
        game_entity_destroy(tree, heap);
        return NULL;
    }

    game_object_set_local_position(trunk, 0.0f, 0.75f, 0.0f);
    game_object_set_size(trunk, 0.0f, 1.5f, 0.0f);
    game_object_set_radius(trunk, 0.15f);
    game_object_set_color(trunk, 0.55f, 0.27f, 0.07f);

    if (!game_entity_attach_part_collider(tree,
                                          trunk,
                                          heap,
                                          COLLISION_CLASS_STATIC,
                                          COLLISION_SHAPE_CUBE)) {
        game_entity_destroy(tree, heap);
        return NULL;
    }

    game_object_set_collider_box(trunk, 0.2f, 0.75f, 0.2f);

    head = game_entity_add_primitive(tree, "TreeHead", GAME_PRIMITIVE_SPHERE);
    if (!head) {
        game_entity_destroy(tree, heap);
        return NULL;
    }

    game_object_set_local_position(head, 0.0f, 1.8f, 0.0f);
    game_object_set_radius(head, 0.55f);
    game_object_set_color(head, 0.0f, 0.6f, 0.0f);

    if (!game_entity_attach_part_collider(tree,
                                          head,
                                          heap,
                                          COLLISION_CLASS_STATIC,
                                          COLLISION_SHAPE_SPHERE)) {
        game_entity_destroy(tree, heap);
        return NULL;
    }

    game_object_set_collider_sphere(head, 0.55f);

    game_entity_sync_parts(tree);
    return tree;
}
