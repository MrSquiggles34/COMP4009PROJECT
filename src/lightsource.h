#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H

#include "ofMain.h"

class LightSource {
public:
    glm::vec3 position;
    glm::vec3 direction;
    float intensity;
    glm::vec3 color;
    float radius;
    float height;

    LightSource(glm::vec3 pos, glm::vec3 dir, float inten, glm::vec3 col, float r, float h)
        : position(pos), direction(glm::normalize(dir)), intensity(inten), color(col), radius(r), height(h) {}
};
#endif
