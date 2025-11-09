#ifndef CYLINDER_H
#define CYLINDER_H

#include "ofMain.h"
#include "hittable.h"
#include "ray.h"

class Cylinder : public hittable {
public:
    Cylinder(glm::vec3 c, float r, float h, glm::vec3 col, glm::vec3 ax)
        : center(c), radius(r), height(h), color(col), axis(ax){
    }

    // A cylinder is...
    glm::vec3 center;
    float radius;
    float height;
    glm::vec3 color;
    glm::vec3 axis; // A normalized vec3 for the central axis

    bool hit(const Ray& r, float t_min, float t_max, hit_record& rec) const override {
        int hit_type = 0;
        
        // ---------- Build an orthogonal around cylinder centre axis (where are my lin alg 2 notes???)
        glm::vec3 w = glm::normalize(axis);
        
        // Check the result of the cross product. OTHERWISE WE MIGHT DIVIDE BY ZERO.
        glm::vec3 temp = glm::cross(w, glm::vec3(1, 0, 0));
        if (glm::length(temp) < 1e-3f) {
            temp = glm::cross(w, glm::vec3(0, 1, 0));
        }

        glm::vec3 u = glm::normalize(temp);
        glm::vec3 v = glm::normalize(glm::cross(w, u));
        
        // Cylinder location relative to the ray source
        glm::vec3 oc = r.orig - center;
        
        // Project the ray into cylinder local space
        glm::vec3 local_orig(
            glm::dot(oc, u),
            glm::dot(oc, w),
            glm::dot(oc, v)
        );

        glm::vec3 local_dir(
            glm::dot(r.dir, u),
            glm::dot(r.dir, w),
            glm::dot(r.dir, v)
        );


        // Equation of a finite cylinder is (xE + txD)^2 + (yE + tyD)^2 = r^2
        //a = xD^2 + yD^2, b = 2xExD + 2yEyD, and c = xE^2 + yE^2 - 1.

        float a = local_dir.x * local_dir.x + local_dir.z * local_dir.z;
        float b = 2.0f * (local_orig.x * local_dir.x + local_orig.z * local_dir.z);
        float c = local_orig.x * local_orig.x + local_orig.z * local_orig.z - radius * radius;
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
                float y_local = local_orig.y + t1 * local_dir.y;
                if (y_local >= - height / 2.0f && y_local <= height / 2.0f) {
                    if (t1 < t_cyl)
                    {
                        t_cyl = t1;
                        hit_type = 1;
                    }
                }
            }

            // Check t2
            if (t2 > t_min && t2 < t_max) {
                float y_local = local_orig.y + t2 * local_dir.y;
                if (y_local >=  - height / 2.0f && y_local <= height / 2.0f) {
                    if (t2 < t_cyl) {
                        t_cyl = t2;
                        hit_type = 1;
                    }
                }
            }
        }

        // Top Plane
        if (fabs(local_dir.y) > 1e-6f) {
            // The equation for ray-plane detection is (zmax - zE) / zD
            float t_top = (height / 2.0f - local_orig.y) / local_dir.y;

            if (t_top > t_min && t_top < t_max) {
                glm::vec3 p = local_orig + t_top * local_dir;

                // Discard if the ray-intersection lies outside the radius. Set hit type.
                if (p.x * p.x + p.z * p.z <= radius * radius) {
                    if (t_top < t_cyl) {
                        t_cyl = t_top;
                        hit_type = 2;
                    }
                }
            }

            // Bottom Plane (See top plane)
            float t_bottom = (-height / 2.0f - local_orig.y) / local_dir.y;
            if (t_bottom > t_min && t_bottom < t_max) {
                glm::vec3 p = local_orig + t_bottom * local_dir;
                if (p.x * p.x + p.z * p.z <= radius * radius) {
                    if (t_bottom < t_cyl) {
                        t_cyl = t_bottom;
                        hit_type = 3;
                    }
                }
            }
        }

        // Resolve Collisions in local space then translate to world space
        if (t_cyl < 1e20f) {
            rec.t = t_cyl;
            rec.p = r.orig + t_cyl * r.dir;

            // Initiate the point in local space
            glm::vec3 p_local = local_orig + t_cyl * local_dir;
            glm::vec3 n_local;

            // Determine surface normal
            if (hit_type == 2) {
                n_local = glm::vec3(0, 1, 0); // top
            }
            else if (hit_type == 3) {
                n_local = glm::vec3(0, -1, 0); // bottom
            }
            else {
                n_local = glm::normalize(glm::vec3(p_local.x, 0.0f, p_local.z)); // side
            }

            // Translate to world space
            glm::vec3 n_world =
                n_local.x * u +
                n_local.y * w +
                n_local.z * v;

            rec.normal = glm::normalize(n_world);
            rec.color = color;
            return true;
        }

        return false;
    }
};

#endif
