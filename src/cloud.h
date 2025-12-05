#ifndef CLOUD_H
#define CLOUD_H

#include "ray.h"
#include <vector>
#include <memory>

// Forward declaration
class LightningSegment;

class Cloud {
public:
    glm::vec3 center;
    glm::vec3 size;  
    float density;
    glm::vec3 color;
    
    Cloud(const glm::vec3& c, const glm::vec3& s, float d, const glm::vec3& col)
        : center(c), size(s), density(d), color(col) {}
    
    bool contains(const glm::vec3& point) const {
        glm::vec3 localPoint = point - center;
        return (fabs(localPoint.x) <= size.x * 0.5f &&
                fabs(localPoint.y) <= size.y * 0.5f &&
                fabs(localPoint.z) <= size.z * 0.5f);
    }
    
    float noise3D(const glm::vec3& p) const {
        glm::vec3 i = glm::floor(p);
        glm::vec3 f = glm::fract(p);
        
        f = f * f * (3.0f - 2.0f * f);
        
        float n = i.x + i.y * 57.0f + i.z * 113.0f;
        
        float a = sin(n) * 43758.5453f;
        float b = sin(n + 57.0f) * 43758.5453f;
        float c = sin(n + 113.0f) * 43758.5453f;
        float d = sin(n + 170.0f) * 43758.5453f;
        
        a = a - floor(a);
        b = b - floor(b);
        c = c - floor(c);
        d = d - floor(d);
        
        float res = glm::mix(glm::mix(a, b, f.x), glm::mix(c, d, f.x), f.y);
        res = glm::mix(res, glm::mix(a, b, f.z), f.z);
        
        return res;
    }
    
    float fbm(const glm::vec3& p, int octaves = 5) const {
        float value = 0.0f;
        float amplitude = 0.5f;
        float frequency = 1.0f;
        
        for (int i = 0; i < octaves; i++) {
            value += amplitude * noise3D(p * frequency);
            frequency *= 2.0f;
            amplitude *= 0.5f;
        }
        
        return value;
    }
    
    float getDensity(const glm::vec3& point) const {
        if (!contains(point)) return 0.0f;
        
        glm::vec3 localPoint = (point - center) / size;
        
        float heightFactor = (localPoint.y + 0.5f);
        
        float edgeFalloff = 1.0f;
        edgeFalloff *= smoothstep(0.5f, 0.1f, fabs(localPoint.x));
        edgeFalloff *= smoothstep(0.5f, 0.05f, fabs(localPoint.z));
        
        float verticalFalloff = smoothstep(1.0f, -0.3f, heightFactor);
        verticalFalloff = glm::pow(verticalFalloff, 0.5f);
        
        float noise1 = fbm(point * 1.0f, 6);
        float noise2 = fbm(point * 3.0f, 4);
        float noise3 = fbm(point * 8.0f, 3);
        
        float combinedNoise = noise1 * 0.6f + noise2 * 0.3f + noise3 * 0.1f;
        
        float turbulence = fbm(point * 2.0f + glm::vec3(100.0f), 5);
        combinedNoise = glm::mix(combinedNoise, turbulence, 0.5f);
        
        float stormDensity = 1.0f - heightFactor * 0.3f;
        stormDensity = glm::pow(stormDensity, 1.2f);
        
        float finalDensity = density * combinedNoise * edgeFalloff * verticalFalloff * stormDensity;
        
        finalDensity = glm::pow(finalDensity, 0.5f) * 1.8f;
        
        return glm::clamp(finalDensity, 0.0f, 2.5f);
    }
    
    bool intersect(const Ray& r, float& tMin, float& tMax) const {
        glm::vec3 boxMin = center - size * 0.5f;
        glm::vec3 boxMax = center + size * 0.5f;
        
        glm::vec3 invDir = 1.0f / r.dir;
        glm::vec3 t0 = (boxMin - r.orig) * invDir;
        glm::vec3 t1 = (boxMax - r.orig) * invDir;
        
        glm::vec3 tMinVec = glm::min(t0, t1);
        glm::vec3 tMaxVec = glm::max(t0, t1);
        
        tMin = glm::max(glm::max(tMinVec.x, tMinVec.y), tMinVec.z);
        tMax = glm::min(glm::min(tMaxVec.x, tMaxVec.y), tMaxVec.z);
        
        return tMax > tMin && tMax > 0.0f;
    }
    
private:
    float smoothstep(float edge0, float edge1, float x) const {
        float t = glm::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }
};

class LightningSegment;

inline glm::vec3 renderVolume(const Ray& r, const std::vector<Cloud>& clouds, 
                              float maxDist, const glm::vec3& backgroundColor,
                              const std::vector<std::shared_ptr<LightningSegment>>& lightningSegs) {
    const float STEP_SIZE = 0.05f;
    
    glm::vec3 color = backgroundColor;
    float transmittance = 1.0f;
    
    for (const auto& cloud : clouds) {
        float tMin, tMax;
        if (!cloud.intersect(r, tMin, tMax)) continue;
        
        tMin = glm::max(tMin, 0.0f);
        tMax = glm::min(tMax, maxDist);
        
        if (tMin >= tMax) continue;
        
        float t = tMin;
        int steps = 0;
        const int MAX_STEPS = 150;
        
        while (t < tMax && transmittance > 0.002f && steps < MAX_STEPS) {
            glm::vec3 pos = r.at(t);
            float localDensity = cloud.getDensity(pos);
            
            if (localDensity > 0.01f) {
                // Lightning illumination (REDUCED REFLECTION)
                float lightningGlow = 0.0f;
                glm::vec3 lightColor = glm::vec3(1.0f, 0.85f, 0.95f);
                
                for (const auto& seg : lightningSegs) {
                    if (seg->isEmissive()) {
                        float glow = seg->computeGlow(pos);
                        lightningGlow += glow;
                    }
                }
                
                float absorption = exp(-localDensity * STEP_SIZE * 25.0f);
                
                // Reduced ambient light
                glm::vec3 ambient = glm::vec3(0.01f, 0.015f, 0.02f); // Was 0.03, 0.04, 0.05
                
                // REDUCED scattered light multiplier (was 2.0f, now 0.5f)
                glm::vec3 scattered = lightColor * lightningGlow * localDensity * 0.5f;
                
                // REDUCED edge lighting (was 0.08f, now 0.02f)
                float edgeLight = glm::pow(1.0f - localDensity, 3.0f) * 0.02f;
                
                glm::vec3 cloudColor = (cloud.color * 0.5f + ambient) * (1.0f + edgeLight) + scattered;
                
                // REDUCED overall contribution (was 6.0f, now 2.0f)
                float contribution = transmittance * (1.0f - absorption) * localDensity;
                color += cloudColor * contribution * 2.0f;
                
                transmittance *= absorption;
            }
            
            t += STEP_SIZE;
            steps++;
        }
    }
    
    return color;
}
#endif