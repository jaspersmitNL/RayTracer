#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "vendor/stb_image_write.h"
#include "vendor/BS_thread_pool.hpp"
#include "scene.hpp"
#include "camera.hpp"

#define WIDTH 1080
#define HEIGHT 720

using namespace glm;

BS::thread_pool pool(4);
Scene *scene = new Scene();
Camera *camera = new Camera(45.0f, 0.1f, 100.0f);


void SetupScene() {
    auto *purple = new Material(vec3(0.8f, 0.2f, 0.8f));
    auto *green = new Material(vec3(0.2f, 0.8f, 0.2f));
    auto *red = new Material(vec3(0.8f, 0.2f, 0.2f));

    scene->objects.push_back(new Sphere(glm::vec3(-1.5f, 0.0f, 0.0f), 0.5f, purple));
    scene->objects.push_back(new Sphere(glm::vec3(1.5f, 0.0f, 0.0f), 0.5f, green));

    scene->objects.push_back(new Sphere(glm::vec3(0.0f, -100.5f, -1.0f), 100.0f, red));


    camera->OnResize(WIDTH, HEIGHT);


}


vec3 DoPixel(uint32_t x, uint32_t y) {

    vec3 color(0);


    Ray ray{};
    ray.origin = camera->GetPosition();
    ray.direction = camera->GetRayDirections()[x + y * WIDTH];

    glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));




    HitRecord rec{};
    if (scene->Hit(ray, rec)) {
        float lightIntensity = glm::max(glm::dot(rec.normal, -lightDir), 0.0f);

        color = rec.material->albedo;
        color *= lightIntensity;
    } else {
        vec3 unitDirection = glm::normalize(ray.direction);
        float t = 0.5f * (unitDirection.y + 1.0f);
        color = (1.0f - t) * vec3(1.0f) + t * vec3(0.5f, 0.7f, 1.0f);
    }


    return color;
}


int main() {
    std::cout << "Hello, World!" << std::endl;


    SetupScene();


    auto *data = new unsigned char[WIDTH * HEIGHT * 3];


    //start time
    auto start = std::chrono::high_resolution_clock::now();


    for (uint32_t y = 0; y < HEIGHT; y++) {
        for (uint32_t x = 0; x < WIDTH; x++) {
            glm::vec3 color = DoPixel(x, y);

            data[(y * WIDTH + x) * 3] = (unsigned char) (color.x * 255);
            data[(y * WIDTH + x) * 3 + 1] = (unsigned char) (color.y * 255);
            data[(y * WIDTH + x) * 3 + 2] = (unsigned char) (color.z * 255);


        }
    }




    //find how long it took in miliseconds
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Time taken: " << duration << "ms" << std::endl;


    stbi_flip_vertically_on_write(1);
    stbi_write_png("test.png", WIDTH, HEIGHT, 3, data, WIDTH * 3);

    return 0;
}
