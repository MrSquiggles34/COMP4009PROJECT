#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"

class hit_record {
public:
    glm::vec3 p;
    glm::vec3 normal;
    glm::vec3 color;
    float t;
};

class hittable {
public:
    virtual ~hittable() = default;

    glm::vec3 color;

    virtual bool hit(const Ray& r, float ray_tmin, float ray_tmax, hit_record& rec) const = 0;
};

#endif