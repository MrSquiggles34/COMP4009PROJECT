#ifndef BRANCH_H
#define BRANCH_H

#include "ofMain.h"
#include "lightningsegment.h"

class Branch {
public:
    // Branch Properties
    glm::vec3 startingPos;
    glm::vec3 direction;
    float distance;
    float startingRadius; // Each segment shrinks in radius than the previous

    float branchProbability; // Chance each branch splits
    float meanBranchLength; // Average length of a split branch
    float maxSegmentAngle; 
    float meanSegmentLength;
    float maxBranchAngle;

    glm::vec3 rotationNormal;

	bool isMainBranch = false;
  bool mainBranchHit = false;
	int branchDepth = 0;

    // Output a list of generated segments
    std::vector<std::shared_ptr<LightningSegment>> segments;

    Branch(
        glm::vec3 start,
        glm::vec3 dir,
        float dist,
        float startRadius,
        float branchProb,
        float meanBranchLen,
        float maxSegAngle,
        float meanSegLen,
        float maxBrAngle,
        glm::vec3 rotNormal,
        bool isMainBranch,
		int branchDepth
    );

    void generateBranch();
    std::function<void(const glm::vec3&)> onMainBranchMove;
};

#endif
