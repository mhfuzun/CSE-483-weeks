#ifndef FORCE_H
#define FORCE_H

typedef struct {
    float x;
    float y;
    float z;
    float duration;
} Force;

Force force_create(float x, float y, float z, float duration);

#endif
