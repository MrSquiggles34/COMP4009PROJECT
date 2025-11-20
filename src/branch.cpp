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
    glm::vec3 normal)
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
    rotationNormal(glm::normalize(normal))
{ }

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
            glm::vec3(1.0),  // color
            axis,
            true,
            *lightPtr
        );

        segments.push_back(seg);

        // Branch based off probability
        if (ofRandom(1.0f) < branchProbability) {
            // Create a child branch
            float branchAngle = ofRandom(-maxBranchAngle, maxBranchAngle);
            glm::vec3 branchDir = glm::rotate(direction, glm::radians(branchAngle), rotationNormal);

            float branchDist = meanBranchLength;

            // Gradually change segment properties with each call
            Branch child(next, branchDir, branchDist,
                radius * 0.5f,  
                branchProbability * 0.8,
                meanBranchLength * 0.5,
                maxSegmentAngle * 1.3,
                meanSegmentLength,
                maxBranchAngle,
                rotationNormal);

            child.generateBranch();

            // append child segments to parent
            for (auto& cseg : child.segments)
                segments.push_back(cseg);
        }

        // Advance
        last = next;

        // Shrink the radius of this branch as it grows
        radius -= 0.01f;
        if (radius < 0.01f)
            radius = 0.01f;
    }
}
