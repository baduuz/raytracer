#include "camera.h"
#include "types.h"

#include <string.h>

#define WIDTH 640
#define HEIGHT 480

typedef enum ImageFormat {
    PPM,
    PNG,
    JPG
} image_format_t;

bool str_ends_with(const char* str, const char* suffix)
{
    size_t len = strlen(str);
    size_t suffix_len = strlen(suffix);
    if (len < suffix_len) {
        return false;
    }
    return strncmp(str + len - suffix_len, suffix, suffix_len) == 0;
}

int main(int argc, char** argv)
{
    image_format_t format = PPM;
    const char* path = NULL;
    if (argc == 2) {
        if (str_ends_with(argv[1], ".png")) {
            format = PNG;
        } else if (str_ends_with(argv[1], ".jpg") || str_ends_with(argv[1], ".jpeg")) {
            format = JPG;
        } else if (str_ends_with(argv[1], ".ppm")) {
            format = PPM;
        } else {
            die("unknown format: file must end in .png, .jpg, .jpeg or .ppm.\n");
        }
        path = argv[1];
    } else if (argc > 2) {
        die("usage: %s [output_file]\n", argv[0]);
    }

    scene_t scene = {
        .ambientColor = (color_t) { .r = 20, .g = 0, .b = 20 },
    };

    material_t sphere1Mat = {
        .scattering = 0.0f,
        .reflection = 0.5f,
        .emitting = 0.5f,
        .color = (color_t) { .r = 255, .g = 0, .b = 0 }
    };
    scene.spheres[0] = (sphere_t) {
        .center = (vec3_t) { 0.0f, -1.0f, 0.0f },
        .radius = 1.0f,
        .material = sphere1Mat
    };
    material_t sphere2Mat = {
        .scattering = 0.0f,
        .reflection = 0.5f,
        .emitting = 0.5f,
        .color = (color_t) { .r = 0, .g = 255, .b = 0 }
    };
    scene.spheres[1] = (sphere_t) {
        .center = (vec3_t) { 0.0f, 1.0f, 0.0f },
        .radius = 1.0f,
        .material = sphere2Mat
    };
    scene.sphereNum = 2;

    camera_t camera = camera_init(&(camera_options_t) {
        .origin = (vec3_t) { -5.0f, 0.0f, 0.0f },
        .yaw = 0.0f,
        .pitch = 0.0f,
        .viewPortDistance = 30.0f,
        .width = WIDTH,
        .height = HEIGHT,
        .pixelSize = 0.1f,
        .raysPerPixel = 1,
    });
    camera_take_picture(&camera, &scene, 1);

    FILE* file = stdout;
    switch (format) {
    case PPM: {
        if (path) {
            file = fopen(path, "w");
            if (!file) {
                die("couldn't open %s for writing\n", path);
            }
        }
        camera_generate_ppm(&camera, file);
    } break;
    case PNG:
        camera_generate_png(&camera, path);
        break;
    case JPG:
        camera_generate_jpg(&camera, path);
        break;
    }

    if (file != stdout) {
        fclose(file);
    }
    camera_destroy(&camera);
    return 0;
}
