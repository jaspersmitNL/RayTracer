#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "vendor/stb_image_write.h"
#include "vendor/BS_thread_pool.hpp"
#include "scene.hpp"

#define WIDTH 1080
#define HEIGHT 720

using namespace glm;

BS::thread_pool pool(1);



Scene* scene = new Scene();




void SetupScene() {
    auto* purple = new Material(vec3(0.8f, 0.2f, 0.8f));
    auto* green = new Material(vec3(0.2f, 0.8f, 0.2f));
    auto *red = new Material(vec3(0.8f, 0.2f, 0.2f));


    scene->objects.push_back(new Sphere(glm::vec3(-1.5f, 0.0f, 0.0f), 0.5f, purple));
    scene->objects.push_back(new Sphere(glm::vec3(1.5f, 0.0f, 0.0f), 0.5f, green));


    scene->objects.push_back(new Sphere(glm::vec3(0.0f, -100.5f, -1.0f), 100.0f, red));

}




vec3 DoPixel(vec2 coord) {

    vec3 color(0);



    Ray ray{};
    ray.origin = vec3(0, 0, 2.0f);
    ray.direction = (vec3(coord.x, coord.y, -1));


    HitRecord rec{};
    if(scene->Hit(ray, rec)) {
        color = rec.material->albedo;
    }


    return color;
}


int main() {
    std::cout << "Hello, World!" << std::endl;


    SetupScene();


    auto *data = new unsigned char[WIDTH * HEIGHT * 3];


    //start time
    auto start = std::chrono::high_resolution_clock::now();

    for (int y = 0; y < HEIGHT; y++) {

        pool.submit([y, data]() {
            for (int x = 0; x < WIDTH; x++) {
                glm::vec2 coord = { (float)x / (float)WIDTH, (float)y / (float)HEIGHT };
                coord = coord * 2.0f - 1.0f; // -1 -> 1
                coord.x *= (float)WIDTH / (float)HEIGHT; //aspect ratio

                vec3 color = DoPixel(coord);

                data[(y * WIDTH + x) * 3] = (unsigned char) (color.x * 255);
                data[(y * WIDTH + x) * 3 + 1] = (unsigned char) (color.y * 255);
                data[(y * WIDTH + x) * 3 + 2] = (unsigned char) (color.z * 255);
            }
        });


    }

    pool.wait_for_tasks();

    //find how long it took in miliseconds
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Time taken: " << duration << "ms" << std::endl;


    stbi_write_png("test.png", WIDTH, HEIGHT, 3, data, WIDTH * 3);

    return 0;
}
