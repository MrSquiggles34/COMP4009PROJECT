#ifndef LIGHTNINGSEGMENT_H
#define LIGHTNINGSEGMENT_H

#include "cylinder.h"
#include "lightsource.h"

class LightningSegment : public Cylinder {
public:
    LightningSegment(
        const glm::vec3& center,
        float radius,
        float height,
        const glm::vec3& color,
        const glm::vec3& axis,
        bool hasEmission,
        const LightSource& light
    )
        : Cylinder(center, radius, height, color, axis),
        hasEmission(hasEmission),
        lightSource(std::make_shared<LightSource>(light))
    { }

    // Is it a light?
    bool hasEmission;

    // Attached Light
    std::shared_ptr<LightSource> lightSource;

    // Turn the light on
    bool isEmissive() const {
        return hasEmission && lightSource != nullptr;
    }
};

#endif