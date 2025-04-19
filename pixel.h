#ifndef PIXEL_H_
#define PIXEL_H_

#include "ray.h"
#include "types.h"
#include <stddef.h>

typedef struct Pixel {
    vec3_t position;
    color_t color;
    size_t index;
    uint32_t raysPerPixel;
} pixel_t;

void pixel_init(pixel_t* pixel, ray_t* rays, vec3_t camPosition, vec3_t pixPosition, size_t pixIndex, uint32_t raysPerPixel)
{
    *pixel = (pixel_t) {
        .position = pixPosition,
        .color = (color_t) { 0 },
        .index = pixIndex,
        .raysPerPixel = raysPerPixel,
    };

    vec3_t camToPixDir = vec3_sub(pixPosition, camPosition);
    vec3_normalize(&camToPixDir);

    for (uint32_t rayIndex = 0; rayIndex < raysPerPixel; rayIndex++) {
        ray_init(&rays[pixIndex + rayIndex], camPosition, camToPixDir);
    }
}

void pixel_take_picture(pixel_t* pixel, ray_t* rayBuffer, scene_t* scene, uint32_t depth)
{
    float totalRed = 0;
    float totalGreen = 0;
    float totalBlue = 0;
    for (uint32_t rayIndex = 0; rayIndex < pixel->raysPerPixel; rayIndex++) {
        ray_cast(&rayBuffer[pixel->index + rayIndex], scene, depth);
        totalRed += rayBuffer[pixel->index + rayIndex].color.r;
        totalGreen += rayBuffer[pixel->index + rayIndex].color.g;
        totalBlue += rayBuffer[pixel->index + rayIndex].color.b;
    }
    pixel->color.r = (uint8_t)(totalRed / (float)pixel->raysPerPixel);
    pixel->color.g = (uint8_t)(totalGreen / (float)pixel->raysPerPixel);
    pixel->color.b = (uint8_t)(totalBlue / (float)pixel->raysPerPixel);
}

#endif
