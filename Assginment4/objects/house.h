#ifndef OBJECT_HOUSE_H
#define OBJECT_HOUSE_H

#include "../GameEntity.h"

GameEntity *house_create(CollisionHeap *heap,
                         float x,
                         float y,
                         float z,
                         GameEntityCollisionFn on_collision);

#endif
