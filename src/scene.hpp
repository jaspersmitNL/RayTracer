#pragma once
#include "sphere.hpp"
#include "material.hpp"
#include <vector>

class Scene: public RTObject {

public:
    std::vector<RTObject*> objects;

    bool shouldRender = true;

    int samplesPerPixel = 100;
    int maxDepth = 5;

    Scene() {
    }

    bool Hit(const Ray &r, HitRecord &rec) const override {
        float closest = 1000000.0f;
        bool didHit = false;

        for(int i = 0; i < objects.size(); i++) {
            HitRecord temp{};
            if(objects[i]->Hit(r, temp)) {
                if(temp.t < closest) {
                    closest = temp.t;
                    rec = temp;
                    didHit = true;
                }
            }
        }


        return didHit;
    }
};