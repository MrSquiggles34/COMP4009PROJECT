#ifndef LIGHTNINGSEGMENT_H
#define LIGHTNINGSEGMENT_H

#include "cylinder.h"
#include "lightsource.h"
#include "ray.h"

class LightningSegment : public Cylinder {
public:
	LightningSegment(
		const glm::vec3 & center,
		float radius,
		float height,
		const glm::vec3 & color,
		const glm::vec3 & axis,
		bool hasEmission,
		const LightSource & light)
		: Cylinder(center, radius, height, color, axis)
		, hasEmission(hasEmission)
		, lightSource(std::make_shared<LightSource>(light)) { }

	// Is it a light?
	bool hasEmission;
	glm::vec3 startPoint;
	glm::vec3 endPoint;

	// Attached Light
	std::shared_ptr<LightSource> lightSource;

	// Turn the light on
	bool isEmissive() {
		return hasEmission && lightSource != nullptr;
	}
	float computeGlow(const glm::vec3 & samplePoint) const;
	float minDistanceToSegment(const Ray & r) const;
	float computeGlowForRay(const Ray & r) const;
};

#endif
