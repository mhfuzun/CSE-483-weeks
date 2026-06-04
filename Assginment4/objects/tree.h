#ifndef OBJECT_TREE_H
#define OBJECT_TREE_H

#include "../GameEntity.h"

GameEntity *tree_create(CollisionHeap *heap,
                        float x,
                        float y,
                        float z,
                        GameEntityCollisionFn on_collision);

#endif
