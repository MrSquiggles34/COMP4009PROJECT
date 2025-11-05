#ifndef SPHERE_H
#define SPHERE_H

#include "ofMain.h"
#include "ray.h"
#include "hittable.h"

class Sphere : public hittable {
public:
    Sphere(glm::vec3 c, float r, glm::vec3 col) 
        : center(c), radius(r), color(col) {
    }

    glm::vec3 center;
    float radius;
    glm::vec3 color;

    // Sphere intersection
    bool hit(const Ray& r, float t_min, float t_max, hit_record& rec) const override {
        glm::vec3 oc = r.orig - center;
        float a = glm::dot(r.dir, r.dir);
        float b = glm::dot(oc, r.dir); 
        float c = glm::dot(oc, oc) - radius * radius;
        float discriminant = b * b - a * c;

        if (discriminant > 0) {
            float sqrtDisc = sqrt(discriminant);
            float temp = (-b - sqrtDisc) / a;
            if (temp < t_max && temp > t_min) {
                rec.t = temp;
                rec.p = r.orig + rec.t * r.dir;
                rec.normal = (rec.p - center) / (float)radius;
                rec.color = color;
                return true;
            }
            temp = (-b + sqrtDisc) / a;
            if (temp < t_max && temp > t_min) {
                rec.t = temp;
                rec.p = r.orig + rec.t * r.dir;
                rec.normal = (rec.p - center) / (float)radius;
                rec.color = color;
                return true;
            }
        }
        return false;
    }
};

#endif