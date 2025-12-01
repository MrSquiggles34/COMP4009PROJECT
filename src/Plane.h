#pragma once
#include <glm/glm.hpp>
#include "ray.h"
#include "hittable.h"

class Plane : public hittable {
public:
    glm::vec3 point;     
    glm::vec3 normal;    
    glm::vec3 color;    

    Plane(const glm::vec3& p, const glm::vec3& n, const glm::vec3& c)
        : point(p), normal(glm::normalize(n)), color(c) {}

    virtual bool hit(const Ray& r, float tMin, float tMax, hit_record& rec) const override {
        float denom = glm::dot(normal, r.direction());

         if (fabs(denom) < 1e-6f)
            return false;

        float t = glm::dot(point - r.origin(), normal) / denom;

        if (t < tMin || t > tMax)
            return false;

        rec.t = t;
        rec.p = r.at(t);
        rec.normal = normal;
        rec.color = color;
        return true;
    }
};
