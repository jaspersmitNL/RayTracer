#pragma once
#include "material.hpp"
#include <glm/glm.hpp>

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;

};



struct HitRecord {
    float t;
    glm::vec3 p;
    glm::vec3 normal;
    Material* material;
    bool didHit;
};