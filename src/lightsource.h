#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H

#include "ofMain.h"

class LightSource {
public:
    glm::vec3 position;
    float intensity = 1.0f;     // scalar intensity (used with attenuation)
    glm::vec3 color = glm::vec3(1.0f); // light color
    float radius = 0.0f;        // small radius for softening (optional radial jitter)

    LightSource() = default;

    LightSource(const glm::vec3& pos, float intensity = 1.0f, const glm::vec3& color = glm::vec3(1.0f), float radius = 0.0f)
        : position(pos), intensity(intensity), color(color), radius(radius) {}
};

#endif
