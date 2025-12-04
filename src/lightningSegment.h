#ifndef LIGHTNINGSEGMENT_H
#define LIGHTNINGSEGMENT_H

#include "cylinder.h"
#include "lightsource.h"
#include <memory>

// LightningSegment inherits Cylinder (which is a hittable)
class LightningSegment : public Cylinder {
public:
    LightningSegment(
        const glm::vec3& center,
        float radius,
        float height,
        const glm::vec3& color,
        const glm::vec3& axis,
        bool hasEmission,
        const LightSource& light,
		bool isMainBranchSegment = false
    )
        : Cylinder(center, radius, height, color, axis),
          hasEmission(hasEmission),
          startPoint(center - glm::normalize(axis) * (height * 0.5f)),
          endPoint(center + glm::normalize(axis) * (height * 0.5f)),
          lightSource(std::make_shared<LightSource>(light)),
          isMainBranchSegment(isMainBranchSegment)
    { }

    // Is it a light?
    bool hasEmission;
    glm::vec3 startPoint;
    glm::vec3 endPoint;

    // Attached light
    std::shared_ptr<LightSource> lightSource;

    // Turn the light on
    bool isEmissive() const {
        return hasEmission && lightSource != nullptr;
    }

    float computeGlow(const glm::vec3 & samplePoint) const;
	float minDistanceToSegment(const Ray & r) const;
	float computeGlowForRay(const Ray & r) const;

    glm::vec3 midpoint() const {
        return 0.5f * (startPoint + endPoint);
    }

    float length() const {
        return glm::length(endPoint - startPoint);
    }

    // Override hit to propagate emission information
    bool hit(const Ray& r, float t_min, float t_max, hit_record& rec) const override {
        // Use Cylinder's hit logic to determine geometry intersection.
        // We call Cylinder::hit (which returns color/normal/t).
        if (!Cylinder::hit(r, t_min, t_max, rec))
            return false;

        // If this segment is emissive, set emission fields in the record
        if (hasEmission && lightSource) {
            rec.emissive = true;
            rec.emissionColor = lightSource->color * lightSource->intensity;
        } else {
            rec.emissive = false;
            rec.emissionColor = glm::vec3(0.0f);
        }

        return true;
    }

    bool isMainBranchSegment;
};

#endif
