#ifndef RAY_H_
#define RAY_H_

#include <stdlib.h>

#include "log.h"
#include "types.h"

#define NULL_TOLERANCE 0.000001

typedef struct Ray {
    vec3_t origin;
    vec3_t direction;
    color_t color;
    uint32_t bounceCount;
    bool rayLostInTheWorld; // Wenn Ray ins nix geht
} ray_t;

void ray_init(ray_t* ray, vec3_t origin, vec3_t direction)
{
    ray->origin = origin;
    ray->direction = direction;
    vec3_normalize(&ray->direction);
    ray->bounceCount = 0;
    ray->rayLostInTheWorld = false;
    ray->color = (color_t) { 0 };
}

collision_t ray_sphere_collision_test(ray_t* ray, sphere_t* sphere)
{
    collision_t collision;
    collision.collided = 0;
    float t0;
    float t1;
    float b = 2.0f * (ray->origin.x - sphere->center.x) * ray->direction.x + 2.0f * (ray->origin.y - sphere->center.y) * ray->direction.y + 2.0f * (ray->origin.z - sphere->center.z) * ray->direction.z;
    float c = (ray->origin.x - sphere->center.x) * (ray->origin.x - sphere->center.x) + (ray->origin.y - sphere->center.y) * (ray->origin.y - sphere->center.y) + (ray->origin.z - sphere->center.z) * (ray->origin.z - sphere->center.z) - (sphere->radius * sphere->radius);
    float root = b * b - 4 * c;
    if (root < (-NULL_TOLERANCE))
        return collision;
    if (root > NULL_TOLERANCE) {
        t0 = (-b + sqrtf(root)) / 2.0f;
        t1 = (-b - sqrtf(root)) / 2.0f;
        if (t1 < t0)
            t0 = t1;
    } else {
        t0 = -b / 2.0f;
    }
    collision.collided = 1;
    collision.distance = t0;
    collision.position.x = ray->origin.x + ray->direction.x * t0;
    collision.position.y = ray->origin.y + ray->direction.y * t0;
    collision.position.z = ray->origin.z + ray->direction.z * t0;
    collision.normal.x = collision.position.x - sphere->center.x;
    collision.normal.y = collision.position.y - sphere->center.y;
    collision.normal.z = collision.position.z - sphere->center.z;
    vec3_normalize(&collision.normal);
    collision.material = sphere->material;
    return collision;
}

void ray_cast(ray_t* ray, scene_t* scene, uint32_t depth)
{
    material_t* bounceMaterials = calloc(depth, sizeof(material_t));

    for (uint32_t bounce = 0; bounce < depth; bounce++) {
        vec3_t dirSphecular;
        // vec3_t dirScattered;
        collision_t nearestCollision;
        nearestCollision.distance = 0;
        ray->rayLostInTheWorld = 1;

        for (uint32_t sphereIndex = 0; sphereIndex < scene->sphereNum; sphereIndex++) {
            collision_t crntCollision = ray_sphere_collision_test(ray, &scene->spheres[sphereIndex]);
            if (crntCollision.collided == 1) {
                ray->rayLostInTheWorld = 0;
                if (crntCollision.distance > nearestCollision.distance) {
                    nearestCollision = crntCollision;
                }
            }
        }

        ray->bounceCount++;

        if (ray->rayLostInTheWorld == 1) {
            bounceMaterials[bounce].reflection = 1;
            bounceMaterials[bounce].emitting = 1;
            bounceMaterials[bounce].color = scene->ambientColor;
            break;
        }

        bounceMaterials[ray->bounceCount - 1] = nearestCollision.material;

        if (bounce + 1 == depth)
            break;

        ray->origin = nearestCollision.position;
        float dotRayNormal = ray->direction.x * nearestCollision.normal.x + ray->direction.y * nearestCollision.normal.y + ray->direction.z * nearestCollision.normal.z;
        dirSphecular.x -= 2 * dotRayNormal * nearestCollision.normal.x;
        dirSphecular.y -= 2 * dotRayNormal * nearestCollision.normal.y;
        dirSphecular.z -= 2 * dotRayNormal * nearestCollision.normal.z;
        ray->direction = dirSphecular;
    }

    float red = 0.0f;
    float green = 0.0f;
    float blue = 0.0f;
    float light = 0.0f;
    for (uint32_t bounceInd = 0; bounceInd < ray->bounceCount; bounceInd++) {
        light += bounceMaterials[bounceInd].emitting;
        red += bounceMaterials[bounceInd].color.r * bounceMaterials[bounceInd].reflection;
        green += bounceMaterials[bounceInd].color.g * bounceMaterials[bounceInd].reflection;
        blue += bounceMaterials[bounceInd].color.b * bounceMaterials[bounceInd].reflection;
    }
    ray->color.r = (uint8_t)((red / (float)ray->bounceCount) * (light / (float)ray->bounceCount));
    ray->color.g = (uint8_t)((green / (float)ray->bounceCount) * (light / (float)ray->bounceCount));
    ray->color.b = (uint8_t)((blue / (float)ray->bounceCount) * (light / (float)ray->bounceCount));

    free(bounceMaterials);
}

#endif
