#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H

#include "ofMain.h"

class LightSource {
public:
    glm::vec3 position;
    float intensity;
    glm::vec3 color;

    LightSource(glm::vec3 pos)
        : position(pos) {}
};
#endif
