#ifndef CYLINDER_H
#define CYLINDER_H

#include "ofMain.h"
#include "hittable.h"
#include "ray.h"

class Cylinder : public hittable {
public:
    Cylinder(glm::vec3 c, float r, float h, glm::vec3 col)
        : center(c), radius(r), height(h), color(col) {
    }

    // A cylinder is...
    glm::vec3 center;
    float radius;
    float height;
    glm::vec3 color;

    bool hit(const Ray& r, float t_min, float t_max, hit_record& rec) const override {
        glm::vec3 oc = r.orig - center;

        // Equation of a finite cylinder is (xE + txD)^2 + (yE + tyD)^2 = r^2
        //a = xD^2 + yD^2, b = 2xExD + 2yEyD, and c = xE^2 + yE^2 - 1.

        float a = r.dir.x * r.dir.x + r.dir.z * r.dir.z;
        float b = 2.0f * (oc.x * r.dir.x + oc.z * r.dir.z);
        float c = oc.x * oc.x + oc.z * oc.z - radius * radius;
        float discriminant = b * b - 4.0f * a * c;

        // Set the collision to infinity
        float t_cyl = 1e20f;

        // Cylinder side collision.
        // Skip if no discriminant or 
        if (discriminant > 0.0f && fabs(a) > 1e-6f) {
            // Find the two roots
            float sqrtD = sqrt(discriminant);
            float t1 = (-b - sqrtD) / (2.0f * a);
            float t2 = (-b + sqrtD) / (2.0f * a);

            // Check t1 is the acceptable collision point
            if (t1 > t_min && t1 < t_max) {
                // Discard if the y-value of the ray-intersection lies beyond the cylinder's height
                float y = r.orig.y + t1 * r.dir.y;
                if (y >= center.y - height / 2.0f && y <= center.y + height / 2.0f) {
                    if (t1 < t_cyl) t_cyl = t1;
                }
            }

            // Check t2
            if (t2 > t_min && t2 < t_max) {
                float y = r.orig.y + t2 * r.dir.y;
                if (y >= center.y - height / 2.0f && y <= center.y + height / 2.0f) {
                    if (t2 < t_cyl) t_cyl = t2;
                }
            }
        }

        // Top Plane
        if (fabs(r.dir.y) > 1e-6f) {
            // The equation for ray-plane detection is (zmax - zE) / zD
            float t_top = (center.y + height / 2.0f - r.orig.y) / r.dir.y;

            if (t_top > t_min && t_top < t_max) {
                glm::vec3 p = r.orig + t_top * r.dir;

                // Discard if the ray-intersection lies outside the radius
                float dx = p.x - center.x;
                float dz = p.z - center.z;
                if (dx * dx + dz * dz <= radius * radius) {
                    if (t_top < t_cyl) t_cyl = t_top;
                }
            }

            // Bottom Plane (See top plane)
            float t_bottom = (center.y - height / 2.0f - r.orig.y) / r.dir.y;
            if (t_bottom > t_min && t_bottom < t_max) {
                glm::vec3 p = r.orig + t_bottom * r.dir;
                float dx = p.x - center.x;
                float dz = p.z - center.z;
                if (dx * dx + dz * dz <= radius * radius) {
                    if (t_bottom < t_cyl) t_cyl = t_bottom;
                }
            }
        }

        // Resolve Collisions
        if (t_cyl < 1e20f) {
            rec.t = t_cyl;
            rec.p = r.orig + t_cyl * r.dir;

            // Determine surface normal
            if (fabs(rec.p.y - (center.y + height / 2.0f)) < 1e-3f)
                rec.normal = glm::vec3(0, 1, 0); // top 
            else if (fabs(rec.p.y - (center.y - height / 2.0f)) < 1e-3f)
                rec.normal = glm::vec3(0, -1, 0); // bottom 
            else
                rec.normal = glm::normalize(glm::vec3(rec.p.x - center.x, 0.0f, rec.p.z - center.z)); // side

            rec.color = color;
            return true;
        }

        return false;
    }
};

#endif
