#pragma once

struct HitRecord;
struct Ray;


class RTObject {
public:
    virtual bool Hit(const Ray& r, HitRecord& rec) const = 0;
};