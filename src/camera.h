#ifndef CAMERA_H
#define CAMERA_H

#include "ray.h"

class Camera {
public:
	glm::vec3 camera_center;
	glm::vec3 lowerLeft;
	glm::vec3 horizontal;
	glm::vec3 vertical;

    // Some hard-coded camera values
    double focalLength = 1.0f;
    double viewportHeight = 2.0f;
    double aspectRatio = 7.0f / 9.0f;

    Camera() {
        double viewportWidth = aspectRatio * viewportHeight;

        camera_center = glm::vec3(0, 0, 0);
        horizontal = glm::vec3(viewportWidth, 0, 0);
        vertical = glm::vec3(0, viewportHeight, 0);
        // Bottom left point of the image plane. Vital for mapping the plane.
        lowerLeft = camera_center - horizontal / 2 - vertical / 2 - ofVec3f(0, 0, focalLength);
    }

    // Create a normalized ray from the camera to every pixel on the viewport plane.
    // Each pixel position u, v is in range [0,1], where (0,0) is bottom left and (1,1) is top right
    Ray getRay(double u, double v) const {
        glm::vec3 dir = glm::normalize(lowerLeft + u * horizontal + v * vertical - camera_center);
        return Ray(camera_center, dir);
    }

};

#endif