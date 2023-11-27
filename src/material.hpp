#pragma once
#include <glm/glm.hpp>

struct Material {
    glm::vec3 albedo;

    explicit Material(const glm::vec3 &albedo) : albedo(albedo) {}

};