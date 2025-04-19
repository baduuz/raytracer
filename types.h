#ifndef TYPES_H_
#define TYPES_H_

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct Vec3 {
    float x;
    float y;
    float z;
} vec3_t;

typedef struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} color_t;

typedef struct Material {
    color_t color;
    float emitting;
    float reflection;
    float scattering;
} material_t;

typedef struct Collision {
    vec3_t position;
    vec3_t normal;
    material_t material;
    bool collided;
    float distance;
} collision_t;

typedef struct Sphere {
    vec3_t center;
    float radius;
    material_t material;
} sphere_t;

typedef struct Scene {
    sphere_t spheres[10];
    color_t ambientColor;
    uint32_t sphereNum;
} scene_t;

static inline vec3_t vec3_sub(vec3_t a, vec3_t b)
{
    return (vec3_t) {
        .x = a.x - b.x,
        .y = a.y - b.y,
        .z = a.z - b.z,
    };
}

static inline float vec3_dot(vec3_t a, vec3_t b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline float vec3_len(vec3_t v)
{
    return sqrtf(vec3_dot(v, v));
}

static inline void vec3_normalize(vec3_t* v)
{
    float len = vec3_len(*v);
    v->x /= len;
    v->y /= len;
    v->z /= len;
}

static inline void vec3_scale(vec3_t* v, float scalar)
{
    v->x *= scalar;
    v->y *= scalar;
    v->z *= scalar;
}

#endif
