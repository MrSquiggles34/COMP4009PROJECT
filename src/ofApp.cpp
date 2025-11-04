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
	cam.camera_center = glm::vec3(0, 0, 3.0f);
	cam.lowerLeft = cam.camera_center - cam.horizontal / 2.0f - cam.vertical / 2.0f - glm::vec3(0, 0, cam.focalLength);

	// Fill the scene
	spheres.push_back(Sphere(glm::vec3(0, 0, 0), 1.0f, glm::vec3(1.0f, 0.5f, 0.0f))); 
	
	pixels.allocate(screenWidth, screenHeight, OF_IMAGE_COLOR); // RGB image

}

//--------------------------------------------------------------
void ofApp::update(){
	// Establish a unit of time for animations
	double frameTime = 1.0f / 30.0f; // Time per frame
	double elapsedTime = frameCount * frameTime; // Time since start

}

//--------------------------------------------------------------
void ofApp::draw(){

	// Body
	for (int y = 0; y < screenHeight; y++) {
		for (int x = 0; x < screenWidth; x++) {
			glm::vec3 color = tracePixel(x, y, frameCount);
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
	double u = double(x) / (screenWidth - 1);
	double v = double(y) / (screenHeight - 1);

	Ray r = cam.getRay(u, v);

	glm::vec3 background(1.0f, 1.0f, 1.0f);
	glm::vec3 color = background;
	double tMin = 1e20; // substitutes infinity

	for (auto& sphere : spheres) {
		double t;
		if (sphere.intersect(r, t) && t < tMin) {
			tMin = t;
			glm::vec3 hitPoint = r.orig + t * r.dir;
			glm::vec3 normal = glm::normalize(hitPoint - sphere.center);
			glm::vec3 lightDir = glm::normalize(cam.camera_center);
			double diff = glm::max(glm::dot(normal, lightDir), 0.0f);
			color = sphere.color * diff;
		}
	}

	return color;
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
