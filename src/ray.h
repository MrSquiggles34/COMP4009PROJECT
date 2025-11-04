#ifndef RAY_H
#define RAY_H

#include "ofMain.h"

class Ray {
public:
    Ray() {}

    Ray(const glm::vec3 origin, const glm::vec3& direction) : orig(origin), dir(direction) {}

    const glm::vec3& origin() const { return orig; }
    const glm::vec3& direction() const { return dir; }

    glm::vec3 at(double t) const {
        return orig + t * dir;
    }

    // Members
    glm::vec3 orig;
    glm::vec3 dir;
};

#endif