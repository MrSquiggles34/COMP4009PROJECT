#include "branch.h"
#include "ofMath.h"     

// Constructor
Branch::Branch(
    glm::vec3 start,
    glm::vec3 dir,
    float dist,
    float radius,
    float branchProb,
    float meanBranchLen,
    float maxSegAngle,
    float meanSegLen,
    float maxBrAngle,
    glm::vec3 normal,
    bool mainBranch,
	int depth = 0)
    :
    startingPos(start),
    direction(glm::normalize(dir)),
    distance(dist),
    startingRadius(radius),
    branchProbability(branchProb),
    meanBranchLength(meanBranchLen),
    maxSegmentAngle(maxSegAngle),
    meanSegmentLength(meanSegLen),
    maxBranchAngle(maxBrAngle),
    rotationNormal(glm::normalize(normal)),
	isMainBranch(mainBranch),
	branchDepth(depth) { }

// Recursively generate another branch
void Branch::generateBranch() {

    glm::vec3 next = startingPos;
    glm::vec3 last = startingPos;
    float radius = startingRadius;

    while (glm::distance(startingPos, next) < distance) {

        // Generate a random angle
        float angle = ofRandom(-maxSegmentAngle, maxSegmentAngle);

        // Generate a random segment length
        float length = ofRandom(0, meanSegmentLength * 2.0f);

        // Rotate the segment around
        glm::vec3 newDir = 
            glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(angle), rotationNormal) * glm::vec4(direction, 0.0));

        // Increment end point
        glm::vec3 step = newDir * length;
        next = last + step;

        // Add a new lightning segment
        glm::vec3 start = last;
        glm::vec3 end = next;
        glm::vec3 center = (start + end) * 0.5f; 
        glm::vec3 axis = glm::normalize(end - start);
        float height = glm::distance(start, end);

        auto lightPtr = std::make_shared<LightSource>(center);
        auto seg = std::make_shared<LightningSegment>(
            center,
            radius,
            height,
            glm::vec3(1.0),  
            axis,
            true,
            *lightPtr
        );
        seg->isMainBranchSegment = isMainBranch; 
		seg->startPoint = start;
		seg->endPoint = end;

		seg->branchDepth = branchDepth;

        segments.push_back(seg);

        // Branch based off probability
        if (ofRandom(1.0f) < branchProbability) {
            if (!isMainBranch || !mainBranchHit) {
                // Create a child branch
                float branchAngle = ofRandom(-maxBranchAngle, maxBranchAngle);
                glm::vec3 branchDir = glm::rotate(direction, glm::radians(branchAngle), rotationNormal);

                float branchDist = meanBranchLength;

                // Gradually change segment properties with each call
                Branch child(next, branchDir, branchDist,
                    radius * 0.5f,
                    branchProbability * 0.4f,
                    meanBranchLength * 0.5,
                    maxSegmentAngle * 1.3,
                    meanSegmentLength,
                    maxBranchAngle,
                    rotationNormal,
                    false,
                    branchDepth + 1
                );

                child.generateBranch();

                // append child segments to parent
                for (auto& cseg : child.segments)
                    segments.push_back(cseg);
            }
        }

        // Advance
        last = next;

        // Shrink the radius of this branch as it grows
        radius -= 0.0005f;
        if (radius < 0.01f)
            radius = 0.01f;
    }
}

float LightningSegment::computeGlow(const glm::vec3 & samplePoint) const {
	float g = 0.75f;
	float li = 2.0f;
	float W = std::max(radius * 12.0f, 0.15f);

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

float LightningSegment::minDistanceToSegment(const Ray & r) const {
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
		float dist = glm::length(glm::cross(d2, p)) / glm::length(d2);
		return dist;
	}

	float t = (d12 * glm::dot(d2, p) - d22 * glm::dot(d1, p)) / denom;
	float s = (d11 * glm::dot(d2, p) - d12 * glm::dot(d1, p)) / denom;

	if (s < 0.0f) s = 0.0f;
	if (s > 1.0f) s = 1.0f;

	t = glm::dot(d1, (p2 + s * d2 - p1)) / d11;

	if (t < 0.0f) {
		glm::vec3 closestSeg = p2 + s * d2;
		return glm::distance(p1, closestSeg);
	}

	glm::vec3 closestRay = p1 + t * d1;
	glm::vec3 closestSeg = p2 + s * d2;
	return glm::distance(closestRay, closestSeg);
}

float LightningSegment::computeGlowForRay(const Ray & r) const {
	float baseGlowMain = 0.08f;
	float baseGlowChild = 0.04f;
	float liMain = 1.2f;
	float liChild = 0.8f;

	float di = minDistanceToSegment(r);

	float W = std::max(radius * 18.0f * (isMainBranchSegment ? 2.5f : 0.3f), 0.12f);

	float power = isMainBranchSegment ? 1.8f : 4.5f;

	float glow = (isMainBranchSegment ? baseGlowMain : baseGlowChild) * (isMainBranchSegment ? liMain : liChild) * expf(-powf(di / W, power));

	glm::vec3 dir = endPoint - startPoint;
	float len2 = glm::dot(dir, dir);
	float t = 0.0f;
	if (len2 > 0.0f) {
		glm::vec3 d = glm::normalize(r.direction());
		glm::vec3 rr = r.origin() - startPoint;
		float denom = len2 - glm::dot(d, dir) * glm::dot(d, dir);
		if (std::abs(denom) < 1e-6f)
			t = glm::dot(rr, dir) / len2;
		else
			t = (glm::dot(rr, dir) - glm::dot(d, dir) * glm::dot(rr, d)) / denom;
		t = glm::clamp(t, 0.0f, 1.0f);
	}

	float tFade = isMainBranchSegment ? (1.0f - 0.3f * t) : powf(1.0f - t, 1.5f);

	float depthFactor = isMainBranchSegment ? 1.0f : powf(0.6f, branchDepth);

	float finalGlow = glow * radius * 18.0f * tFade * depthFactor;

	return finalGlow;
}
