#pragma once

#include "ray.hpp"
#include "scene.hpp"
#include "camera.hpp"
#include <glm/glm.hpp>


#define WIDTH 440
#define HEIGHT 440

namespace RTRenderer {
    using namespace glm;
    vec3 shade(Ray r, HitRecord h, int i, Scene* scene) {
        if(i==0) return glm::vec3(0);
        glm::vec3 lightPos = glm::vec3(0, 3, -7);
        vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
        float lightPointIntensity = 24.0f;
        glm::vec3 color(0.0f);

        float const distance = length(lightPos - h.p);

        vec3 lightDir = glm::normalize(lightPos - h.p);
        float lightIntensity = lightPointIntensity / (distance * distance);



        Ray shadowRay{};
        shadowRay.direction = glm::normalize(h.p - lightPos);
        shadowRay.origin = lightPos;


        HitRecord shadowRec{};
        scene->Hit(shadowRay, shadowRec);

        if(!(shadowRec.didHit && (shadowRec.t < distance - 0.1f))) {
            color = glm::clamp((lightColor * lightIntensity), 0.0f, 1.0f);
        }

        double rX = (float)std::rand() / RAND_MAX;
        double rY = (float)std::rand() / RAND_MAX;
        double rZ = (float)std::rand() / RAND_MAX;
        glm::vec3 rDir(rX, rY, rZ);
        rDir*=2;
        rDir-=glm::vec3(1);
        rDir=glm::normalize(rDir);
        float cos = glm::dot(rDir, h.normal);
        if(cos < 0) {
            rDir*=-1;
            cos *= -1;
        }
        HitRecord bounceHit{};
        Ray bounceRay{};
        bounceRay.direction = rDir;
        bounceRay.origin = h.p + h.normal*0.01f;
        if(scene->Hit(bounceRay, bounceHit)) {
            float in = 1.0f/(bounceHit.t*bounceHit.t);
            if(in > 1.0f) in = 1.0f;
            color+=shade(bounceRay,bounceHit,i-1, scene)*in;

        }

        return glm::clamp(color*h.material->albedo, 0.0f, 1.0f);
    }

    vec3 DoPixel(uint32_t x, uint32_t y, Scene* scene, Camera* camera) {

        vec3 color(0);


        for(int i = 0; i<100; i++) {
            Ray ray{};
            ray.origin = camera->GetPosition();
            ray.direction = camera->GetRayDirections()[x + y * WIDTH];

            HitRecord rec{};
            if (scene->Hit(ray, rec)) {
                color += shade(ray, rec, 5, scene);

            } else {
                vec3 unitDirection = glm::normalize(ray.direction);
                float t = 0.5f * (unitDirection.y + 1.0f);
                color += (1.0f - t) * vec3(1.0f) + t * vec3(0.5f, 0.7f, 1.0f);
            }
        }


        return color/100.0f;
    }

}