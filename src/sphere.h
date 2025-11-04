#pragma once
#include "ofMain.h"
#include "ray.h"

class Sphere {
public:
    Sphere(glm::vec3 c, double r, glm::vec3 col) : center(c), radius(r), color(col) {}

    glm::vec3 center;
    double radius;
    glm::vec3 color;

    // Ray-sphere intersection
    bool intersect(const Ray& r, double& t) const {
        glm::vec3 oc = r.orig - center;
        double a = glm::dot(r.dir, r.dir);
        double b = 2.0f * glm::dot(oc, r.dir);
        double c = glm::dot(oc, oc) - radius * radius;
        double discriminant = b * b - 4 * a * c;

        if (discriminant < 0) return false;
        
        t = (-b - sqrt(discriminant)) / (2.0f * a); // nearest hit
        return t > 0;
    }
};

