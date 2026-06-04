#ifndef OBJECT_PLAYER_H
#define OBJECT_PLAYER_H

#include "../GameEntity.h"

GameEntity *player_create(CollisionHeap *heap,
                          float x,
                          float y,
                          float z,
                          GameEntityCollisionFn on_collision);

#endif
