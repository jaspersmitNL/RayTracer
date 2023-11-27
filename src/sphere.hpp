#pragma once
#include <glm/glm.hpp>
#include "object.hpp"
#include "ray.hpp"


class Sphere: public RTObject {
public:
    glm::vec3 center;
    float radius;
    Material* material;

    Sphere(glm::vec3 center, float radius, Material* material): center(center), radius(radius), material(material) {
    }

    bool Hit(const Ray &r, HitRecord &rec) const override {

        glm::vec3 oc = r.origin - center;

        float a = glm::dot(r.direction, r.direction);
        float b = 2.0f * glm::dot(oc, r.direction);
        float c = glm::dot(oc, oc) - radius * radius;
        float d = b * b - 4.0f * a * c;


        if(d <= 0) {
            return false; // no hit
        }

        float t = (-b - glm::sqrt(d)) / (2.0f * a);

        glm::vec3 p = r.origin + t * r.direction;
        glm::vec3 normal = glm::normalize(p - center);

        rec.t = t;
        rec.p = p;
        rec.normal = normal;
        rec.material = material;
        rec.didHit = true;
        return true;
    }
};