#ifndef CAMERA_H_
#define CAMERA_H_

#include <stdint.h>
#include <stdlib.h>

#include "stb_image_write.h"

#include "log.h"
#include "pixel.h"
#include "ray.h"
#include "types.h"

typedef struct Camera {
    vec3_t origin;
    uint32_t width;
    uint32_t height;
    ray_t* rayBuffer;
    pixel_t* pixelBuffer;
} camera_t;

typedef struct CameraOptions {
    vec3_t origin;
    float yaw;
    float pitch;
    float viewPortDistance;
    uint32_t width;
    uint32_t height;
    float pixelSize;
    uint32_t raysPerPixel;
} camera_options_t;

vec3_t _transform_vec(vec3_t point, float yaw, float pitch)
{
    // TODO: actually do the calculations
    return point;
}

// Kamerarichtung standardmäßig (1, 0, 0)
// size: Hor*Vert
camera_t camera_init(camera_options_t* options)
{
    camera_t camera = {
        .origin = options->origin,
        .width = options->width,
        .height = options->height,
    };
    camera.rayBuffer = calloc(camera.width * camera.height * options->raysPerPixel, sizeof(ray_t));
    if (!camera.rayBuffer) {
        die("Out Of Memory");
    }
    camera.pixelBuffer = calloc(camera.width * camera.height, sizeof(pixel_t));
    if (!camera.pixelBuffer) {
        die("Out Of Memory");
    }

    vec3_t upperLeft;
    vec3_t upperRight;
    vec3_t bottomLeft;

    upperLeft.x = camera.origin.x + options->viewPortDistance;
    upperRight.x = camera.origin.x + options->viewPortDistance;
    bottomLeft.x = camera.origin.x + options->viewPortDistance;

    upperLeft.y = camera.origin.y - ((float)camera.width / 2.0f) * options->pixelSize - options->pixelSize / 2.0f;
    upperRight.y = camera.origin.y + ((float)camera.width / 2.0f) * options->pixelSize - options->pixelSize / 2.0f;
    bottomLeft.y = camera.origin.y - ((float)camera.width / 2.0f) * options->pixelSize - options->pixelSize / 2.0f;

    upperLeft.z = camera.origin.z + ((float)camera.height / 2.0f) * options->pixelSize - options->pixelSize / 2.0f;
    upperRight.z = camera.origin.z + ((float)camera.height / 2.0f) * options->pixelSize - options->pixelSize / 2.0f;
    bottomLeft.z = camera.origin.z - ((float)camera.height / 2.0f) * options->pixelSize - options->pixelSize / 2.0f;

    upperLeft = _transform_vec(upperLeft, options->yaw, options->pitch);
    upperRight = _transform_vec(upperRight, options->yaw, options->pitch);
    bottomLeft = _transform_vec(bottomLeft, options->yaw, options->pitch);

    vec3_t viewportHorAxis;
    viewportHorAxis.x = (upperRight.x - upperLeft.x);
    viewportHorAxis.y = (upperRight.y - upperLeft.y);
    viewportHorAxis.z = (upperRight.z - upperLeft.z);
    vec3_normalize(&viewportHorAxis);
    vec3_scale(&viewportHorAxis, options->pixelSize);
    vec3_t viewportVerAxis;
    viewportVerAxis.x = (bottomLeft.x - upperLeft.x);
    viewportVerAxis.y = (bottomLeft.y - upperLeft.y);
    viewportVerAxis.z = (bottomLeft.z - upperLeft.z);
    vec3_normalize(&viewportVerAxis);
    vec3_scale(&viewportVerAxis, options->pixelSize);

    for (uint32_t y = 0; y < camera.height; y++) {
        for (uint32_t x = 0; x < camera.width; x++) {
            vec3_t pixPos;
            pixPos.x = upperLeft.x + viewportHorAxis.x * (float)x + viewportVerAxis.x * (float)y;
            pixPos.y = upperLeft.y + viewportHorAxis.y * (float)x + viewportVerAxis.y * (float)y;
            pixPos.z = upperLeft.z + viewportHorAxis.z * (float)x + viewportVerAxis.z * (float)y;

            size_t index = y * camera.width + x;
            pixel_init(&camera.pixelBuffer[index], camera.rayBuffer, camera.origin, pixPos,
                index * options->raysPerPixel, options->raysPerPixel);
        }
    }

    return camera;
}

void camera_destroy(camera_t* camera)
{
    free(camera->rayBuffer);
    free(camera->pixelBuffer);
}

void camera_take_picture(camera_t* camera, scene_t* scene, uint32_t depth)
{
    for (uint32_t y = 0; y < camera->height; y++) {
        for (uint32_t x = 0; x < camera->width; x++) {
            pixel_t* crntPixel = &camera->pixelBuffer[y * camera->width + x];
            pixel_take_picture(crntPixel, camera->rayBuffer, scene, depth);
        }
    }
}

void camera_generate_ppm(camera_t* camera, FILE* file)
{
    fprintf(file, "P3\n%d %d\n255\n", camera->width, camera->height);
    for (uint32_t y = 0; y < camera->height; y++) {
        for (uint32_t x = 0; x < camera->width; x++) {
            pixel_t* crntPixel = &camera->pixelBuffer[y * camera->width + x];
            fprintf(file, "%d %d %d\n", crntPixel->color.r, crntPixel->color.g, crntPixel->color.b);
        }
    }
}

uint8_t* _create_buffer(camera_t* camera)
{
    uint8_t* data = malloc(camera->width * camera->height * 3);
    for (uint32_t y = 0; y < camera->height; y++) {
        for (uint32_t x = 0; x < camera->width; x++) {
            pixel_t* pixel = &camera->pixelBuffer[y * camera->width + x];
            size_t index = 3 * (y * camera->width + x);
            data[index + 0] = pixel->color.r;
            data[index + 1] = pixel->color.g;
            data[index + 2] = pixel->color.b;
        }
    }
    return data;
}

void camera_generate_png(camera_t* camera, const char* path)
{
    uint8_t* data = _create_buffer(camera);
    if (!stbi_write_png(path, (int32_t)camera->width, (int32_t)camera->height, 3, data, (int32_t)camera->width * 3)) {
        die("failed writing image to %s\n", path);
    }
    free(data);
}

void camera_generate_jpg(camera_t* camera, const char* path)
{
    uint8_t* data = _create_buffer(camera);
    if (!stbi_write_jpg(path, (int32_t)camera->width, (int32_t)camera->height, 3, data, 90)) {
        die("failed writing image to %s\n", path);
    }
    free(data);
}

#endif
