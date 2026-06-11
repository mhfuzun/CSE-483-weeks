#ifndef __COLOR_H__
#define __COLOR_H__

#include <stdint.h>
#include "types/vector.h"

static inline uint32_t rgb_to_color(vector3_t rgb) {
    float r = clamp01(rgb.x);
    float g = clamp01(rgb.y);
    float b = clamp01(rgb.z);

    uint32_t R = (uint32_t)(r * 255.0f + 0.5f);
    uint32_t G = (uint32_t)(g * 255.0f + 0.5f);
    uint32_t B = (uint32_t)(b * 255.0f + 0.5f);

    return (255u << 24) | (R << 16) | (G << 8) | B;
}

static inline vector3_t color_to_rgb(uint32_t color) {
    vector3_t rgb;

    uint32_t R = (color >> 16) & 0xFF;
    uint32_t G = (color >> 8)  & 0xFF;
    uint32_t B = color         & 0xFF;

    rgb.x = (float)R / 255.0f;
    rgb.y = (float)G / 255.0f;
    rgb.z = (float)B / 255.0f;

    return rgb;
}

#endif // __COLOR_H__
