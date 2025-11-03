#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	// Load resources
	//basic.load("../../src/basic.vert", "../../src/basic.frag");

	// Miscellanous 
	// Disable timing and frame-rate for offline rendering
	ofSetFrameRate(0);
	ofSetVerticalSync(false);

	ofBackground(30.0, 30.0, 30.0, 255);

	// Set values
	screenWidth = ofGetWidth();
	screenHeight = ofGetHeight();

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

	// Save a frame
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
	// Ray tracing properties. See RTIOW
	// Return color 
	return glm::vec3(0.0f, 0.0f, 0.0f); 
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
