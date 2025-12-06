#pragma once

#include "ofMain.h"
#include "camera.h"
#include "sphere.h"
#include "cylinder.h"
#include "lightsource.h"
#include "lightningSegment.h"
#include "branch.h"
#include "Plane.h"
#include "cloud.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		// The Raytracing Algorithm
		glm::vec3 tracePixel(float x, float y, int frame, const std::vector<std::shared_ptr<LightningSegment>>& segs);
		
		// Objects
		ofShader basic;
		ofPixels pixels;
		Camera cam;

		// Scene Data structures
		std::vector<Cloud> clouds;
		std::vector<std::shared_ptr<hittable>> world;
		std::vector<std::shared_ptr<Sphere>> strikeTargets;
		std::vector<LightSource> lightSources;
		std::vector<std::shared_ptr<LightningSegment>> lightningSegments;
		int segmentsToShow = 0;

		
		// Settings
		int screenWidth;
		int screenHeight;
		int frameCount = 0;
		int totalFrames = 24; // 1 seconds at 24 fps

};
