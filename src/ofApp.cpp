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
	cam.camera_center = glm::vec3(0, -1, 3.0f);
	cam.lowerLeft = cam.camera_center - cam.horizontal / 2.0f - cam.vertical / 2.0f - glm::vec3(0, 0, cam.focalLength);

	// Fill the scene
	world.push_back(std::make_shared<Sphere>(glm::vec3(0, 0, -1), 0.5f, glm::vec3(1.0f, 0.5f, 0.0f)));
	world.push_back(std::make_shared<Cylinder>(glm::vec3(2.0f, 2.0, -5.0f), 2.0f, 1.0f, glm::vec3(0.2f, 0.8f, 1.0f), glm::vec3(0.0, 0.0, 1.0)));
	world.push_back(std::make_shared<Cylinder>(glm::vec3(0.0f, 4.0, -5.0f), 2.0f, 1.0f, glm::vec3(0.2f, 0.8f, 1.0f), glm::vec3(1.0, 0.0, 0.5)));

	pixels.allocate(screenWidth, screenHeight, OF_IMAGE_COLOR); // RGB image

}

//--------------------------------------------------------------
void ofApp::update(){
	// Establish a unit of time for animations
	float frameTime = 1.0f / 30.0f; // Time per frame
	float elapsedTime = frameCount * frameTime; // Time since start

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
	float u = float(x) / (screenWidth - 1);
	float v = float(y) / (screenHeight - 1);

	Ray r = cam.getRay(u, v);

	hit_record rec;
	float closest = 1e20; // Infinity

	// Set the background
	glm::vec3 background(0.0f, 0.0f, 0.0f);
	glm::vec3 pixelColor = background;

	// Test for ray-object intersections
	for (auto& obj : world) {
		if (obj->hit(r, 0.001, closest, rec)) {
			closest = rec.t;
			glm::vec3 lightDir = glm::normalize(cam.camera_center); // Light source set as camera
			float diff = glm::max(glm::dot(rec.normal, lightDir), 0.0f);
			float ambient = 0.2;
			pixelColor = rec.color * (diff);
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
