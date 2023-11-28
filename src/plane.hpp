#pragma once
#include "object.hpp"
#include "material.hpp"
#include <glm/glm.hpp>


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
