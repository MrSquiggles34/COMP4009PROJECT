#include "lightningSegment.h"
#include <cmath> // For fabs, expf, powf

LightningSegment::DistInfo LightningSegment::minDistInfo(const Ray & r) const {
	DistInfo info;
	glm::vec3 p1 = r.origin();
	glm::vec3 d1 = r.direction();
	glm::vec3 p2 = startPoint;
	glm::vec3 d2 = endPoint - startPoint;
	glm::vec3 p = p1 - p2;

	float d11 = glm::dot(d1, d1);
	float d22 = glm::dot(d2, d2);
	float d12 = glm::dot(d1, d2);

	float denom = d11 * d22 - d12 * d12;

	if (fabs(denom) < 1e-6f) {
		// Parallel case: project p1 onto infinite line, then clamp to segment
		float t_proj = glm::dot(p, d2) / d22;
		t_proj = glm::clamp(t_proj, 0.0f, 1.0f);
		glm::vec3 closestSeg = p2 + t_proj * d2;
		info.di = glm::distance(p1, closestSeg);
		// Improved: Compute actual t on ray to the closest point
		info.t = glm::dot(closestSeg - p1, d1) / d11;
		if (info.t < 0.0f) {
			info.t = -1.0f; // Behind camera
		}
		return info;
	}

	float s = (d11 * glm::dot(d2, p) - d12 * glm::dot(d1, p)) / denom;
	float t = (d12 * glm::dot(d2, p) - d22 * glm::dot(d1, p)) / denom;

	s = glm::clamp(s, 0.0f, 1.0f);

	// Recompute t with clamped s
	t = glm::dot(d1, (p2 + s * d2 - p1)) / d11;

	if (t < 0.0f) {
		glm::vec3 closestSeg = p2 + s * d2;
		info.di = glm::distance(p1, closestSeg);
		info.t = -1.0f; // Behind camera
	} else {
		glm::vec3 closestRay = p1 + t * d1;
		glm::vec3 closestSeg = p2 + s * d2;
		info.di = glm::distance(closestRay, closestSeg);
		info.t = t;
	}
	return info;
}

// Deprecate minDistanceToSegment and use minDistInfo.di instead
float LightningSegment::minDistanceToSegment(const Ray & r) const {
	return minDistInfo(r).di;
}

// Update computeGlowForRay to use adjusted parameters (reduce g and li for less brightness)
float LightningSegment::computeGlowForRay(const Ray & r) const {
	float g = 0.25f; // Reduced from 0.08 to prevent overbrightness
	float li = 6.0f; // Reduced from 4.0
	float W = glm::max(radius * 3.0f, 0.08f);
	float di = minDistanceToSegment(r);
	float glow = g * li * expf(-powf(di / W, 2.0f));
	return glow;
}

// Use similar parameters for point-based glow (for surfaces)
float LightningSegment::computeGlow(const glm::vec3 & samplePoint) const {
	float g = 0.25f; // Match ray version
	float li = 6.0f; // Match ray version
	float W = glm::max(radius * 12.0f, 0.15f); // Keep broader for surfaces if desired, or match ray
	glm::vec3 segVec = endPoint - startPoint;
	glm::vec3 ptVec = samplePoint - startPoint;
	float segLen2 = glm::dot(segVec, segVec);
	if (segLen2 < 0.0001f) return 0.0f;
	float t = glm::dot(ptVec, segVec) / segLen2;
	t = glm::clamp(t, 0.0f, 1.0f);
	glm::vec3 closest = startPoint + t * segVec;
	float di = glm::distance(samplePoint, closest);
	float glow = g * li * expf(-powf(di / W, 2.0f));
	return glow;
}
