#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	// Load resources
	//basic.load("../../src/basic.vert", "../../src/basic.frag");

	// Miscellanous 
	// Disable timing and frame-rate for offline rendering
	ofSetFrameRate(0);
	ofSetVerticalSync(false);

	// Set values
	screenWidth = ofGetWidth();
	screenHeight = ofGetHeight();
	cam = Camera();
	
	// Reposition camera
	cam.camera_center = glm::vec3(0, 0, 2.0f);
	cam.lowerLeft = cam.camera_center - cam.horizontal / 2.0f - cam.vertical / 2.0f - glm::vec3(0, 0, cam.focalLength);

	// Fill the scene
	world.push_back(std::make_shared<Sphere>(glm::vec3(0, 0, -1), 0.5f, glm::vec3(1.0f, 0.5f, 0.0f)));
	world.push_back(std::make_shared<Cylinder>(glm::vec3(2.0f, 2.0, -5.0f), 2.0f, 1.0f, glm::vec3(0.2f, 0.8f, 1.0f), glm::vec3(0.0, 0.0, 1.0)));
	world.push_back(std::make_shared<Cylinder>(glm::vec3(-3.0f, 3.0, -5.0f), 2.0f, 1.0f, glm::vec3(0.2f, 0.8f, 1.0f), glm::vec3(1.0, 0.0, 0.5)));

	// Generate the strike
	glm::vec3 start(0, -1.5, 0);
	glm::vec3 dir(0, 1, 0);  // pointing downward!!!

	Branch mainBranch(start,
		dir,
		3.0f,      // distance
		0.05f,        // radius
		0.2f,       // branch probability
		0.8f,       // mean branch length
		30.0f,       // max segment angle
		0.08f,        // mean segment length
		50.0f,       // max branch angle
		glm::vec3(0, 0, 1));
	mainBranch.generateBranch();

	// Copy the segments
	for (auto& s : mainBranch.segments)
		lightningSegments.push_back(s);

	pixels.allocate(screenWidth, screenHeight, OF_IMAGE_COLOR); 
}

//--------------------------------------------------------------
void ofApp::update(){
	// Establish a unit of time for animations
	float frameTime = 1.0f / 30.0f; // Time per frame
	float elapsedTime = frameCount * frameTime; // Time since start
}

//--------------------------------------------------------------
void ofApp::draw(){
	for (int y = 0; y < screenHeight; y++) {
		for (int x = 0; x < screenWidth; x++) {
			glm::vec3 color = tracePixel(x, y, frameCount);
			color = glm::clamp(color, 0.0f, 1.0f);
			pixels.setColor(x, y, ofColor(color.r * 255, color.g * 255, color.b * 255));
		}
	}

	// Save a frame and exit
	ofSaveImage(pixels, "../../out/output" + ofToString(frameCount, 5, '0') + ".png");
	frameCount++;

	if (frameCount >= totalFrames) {
		ofExit();
	}

	// Batch script
	// Run this (on windows) with ffmpeg to generate a video using the frames 
	// https://ffmpeg.org/download.html
	// C:\ffmpeg-8.0-essentials_build\bin\ffmpeg.exe -framerate 30 -i out\output%05d.png -c:v libx264 -pix_fmt yuv420p out.mp4
}

glm::vec3 ofApp::tracePixel(int x, int y, int frame) {
	(void)frame; 
	float u = float(x) / (screenWidth - 1);
	float v = float(y) / (screenHeight - 1);

	Ray r = cam.getRay(u, v);

	hit_record rec;
	float closest = 1e20f; // Infinity

	// Set the background
	glm::vec3 background(0.0f, 0.0f, 0.0f);
	glm::vec3 pixelColor = background;

	// Test for ray-object intersections
	for (auto& obj : world) {
		if (obj->hit(r, 0.001f, closest, rec)) {
			closest = rec.t;

			glm::vec3 totalLightRGB(0.0f);
			for (auto& lightningSegment : lightningSegments) {
				// Use the light attached to every lightning segment
				auto& light = *(lightningSegment->lightSource);

				glm::vec3 L = light.position - rec.p;
				float dist2 = glm::dot(L, L);
				float dist = sqrt(dist2);
				if (dist < 1e-4f) continue;
				glm::vec3 lightDir = glm::normalize(L);

				// Lambertian diffuse
				float diff = glm::max(glm::dot(rec.normal, lightDir), 0.0f);

				totalLightRGB += glm::vec3(diff);
			}

			// Ambient (base)
			glm::vec3 ambient = 0.1f * rec.color;

			float brightness = glm::clamp(totalLightRGB.r, 0.0f, 0.9f);
			glm::vec3 diffuse = rec.color * brightness;
			pixelColor = ambient + diffuse;

			// final clamp
			pixelColor = glm::clamp(pixelColor, glm::vec3(0.0f), glm::vec3(1.0f));
		}
	}

	// Test ray against lightning segments
	for (auto& seg : lightningSegments) {
		if (seg->hit(r, 0.001f, closest, rec)) {
			closest = rec.t;

			glm::vec3 baseColor = glm::vec3(1.0f, 1.0f, 1.0f); 

			pixelColor = baseColor;
		}
	}

	return pixelColor;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
