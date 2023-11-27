#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <chrono>
#include <random>


#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "vendor/stb_image_write.h"
#include "vendor/BS_thread_pool.hpp"
#include "scene.hpp"
#include "camera.hpp"

#define WIDTH 1440
#define HEIGHT 1080
using namespace glm;

inline double random_double() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}


class Plane: public RTObject {
public:
    glm::vec3 center;
    glm::vec3 normal;
    Material* material;

    Plane(glm::vec3 center, glm::vec3 normal, Material* material): center(center), normal(normal), material(material) {
    }

    bool Hit(const Ray &r, HitRecord &rec) const override {

        float denom = glm::dot(normal, r.direction);
        if(denom < 1e-6) {
            glm::vec3 p0l0 = center - r.origin;
            float t = glm::dot(p0l0, normal) / denom;
            if(t >= 0) {
                glm::vec3 p = r.origin + t * r.direction;
                glm::vec3 normal = glm::normalize(p - center);

                rec.t = t;
                rec.p = p;
                rec.normal = normal;
                rec.material = material;
                rec.didHit = true;
                return true;
            }
        }

        return false;
    }
};


BS::thread_pool pool(8);
Scene *scene = new Scene();
Camera *camera = new Camera(45.0f, 0.1f, 100.0f);


void SetupScene() {
    auto *purple = new Material(vec3(0.8f, 0.2f, 0.8f));
    auto *green = new Material(vec3(0.2f, 0.8f, 0.2f));
    auto *red = new Material(vec3(0.8f, 0.2f, 0.2f));
    auto *blue = new Material(vec3(0.2f, 0.2f, 0.8f));
    auto* white = new Material(vec3(1.0f, 1.0f, 1.0f));





    vec3 leftWallNormal = vec3(1, 0, 0);
    scene->objects.push_back(new Plane(vec3(-1, 0, 0), leftWallNormal, red));
    vec3 backWallNormal = vec3(0, 0, -1);
    scene->objects.push_back(new Plane(vec3(0, 0, 2), backWallNormal, white));




//    scene->objects.push_back(new Sphere(vec3(0, 0, -1), 0.5f, purple));
//    scene->objects.push_back(new Sphere(vec3(0, -100.5, -1), 100, green));
//    scene->objects.push_back(new Sphere(vec3(1, 0, -1), 0.5f, red));
//    scene->objects.push_back(new Sphere(vec3(-1, 0, -1), 0.5f, blue));
//
//



    camera->OnResize(WIDTH, HEIGHT);


}


vec3 DoPixel(uint32_t x, uint32_t y) {

    vec3 color(0);


    Ray ray{};
    ray.origin = camera->GetPosition();
    ray.direction = camera->GetRayDirections()[x + y * WIDTH];



    glm::vec3 lightPos = glm::vec3(0, 0, 0);
    float lightPointIntensity = 100.0f;


    HitRecord rec{};
    if (scene->Hit(ray, rec)) {

        float const distance = length(rec.t - lightPos);


        vec3 lightDir = glm::normalize(lightPos - rec.p);
        float lightIntensity = glm::dot(lightDir, rec.normal);
        lightIntensity = lightIntensity * (lightPointIntensity /( distance * distance));
        lightIntensity = glm::clamp(lightIntensity, 0.0f, 1.0f);

        color = rec.material->albedo;

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


            vec3 finalColor = DoPixel(x, y);


            data[(y * WIDTH + x) * 3] = (unsigned char) (finalColor.x * 255);
            data[(y * WIDTH + x) * 3 + 1] = (unsigned char) (finalColor.y * 255);
            data[(y * WIDTH + x) * 3 + 2] = (unsigned char) (finalColor.z * 255);
        }

        //print progress every 10%
        if (y % (HEIGHT / 10) == 0)
            std::cout << "Progress: " << (float) y / (float) HEIGHT * 100.0f << "%" << std::endl;


    }


    pool.wait_for_tasks();


    //find how long it took in miliseconds
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Time taken: " << duration << "ms" << std::endl;


    stbi_flip_vertically_on_write(1);
    stbi_write_png("test.png", WIDTH, HEIGHT, 3, data, WIDTH * 3);

    return 0;
}
