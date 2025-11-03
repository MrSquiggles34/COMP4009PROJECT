#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	// Load resources
	basic.load("../../src/basic.vert", "../../src/basic.frag");

	// Miscellanous 
	// Disable timing and frame-rate for offline rendering
	ofSetFrameRate(0);
	ofSetVerticalSync(false);

	ofEnableDepthTest();
	ofBackground(30.0, 30.0, 30.0, 255);

	// Set values
	screenWidth = ofGetWidth();
	screenHeight = ofGetHeight();
	
	demo.set(100.0f);
	cam.setPosition(screenWidth * 0.5, screenHeight * 0.5, 300.0f);
}

//--------------------------------------------------------------
void ofApp::update(){
	// Establish a unit of time for animations
	float frameTime = 1.0f / 30.0f; // Time per frame
	float elapsedTime = frameCount * frameTime; // Time since start

}

//--------------------------------------------------------------
void ofApp::draw(){
	// Header
	cam.begin();
	basic.begin();

	// Initial Shader Uniforms
	glm::mat4 projectionMatrix = cam.getProjectionMatrix();
	basic.setUniformMatrix4f("projectionMatrix", projectionMatrix);

	glm::mat4 worldMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(screenWidth * 0.5, screenHeight * 0.5, 0.0f));
	basic.setUniformMatrix4f("worldMatrix", worldMatrix);
	basic.setUniform3f("objectColor", glm::vec3(255.0, 255.0, 255.0) / 255.0f);
	
	// Body
	demo.draw();

	// Footer
	basic.end();
	cam.end();

	// Save a frame
	ofSaveScreen("../../out/output" + ofToString(frameCount, 5, '0') + ".png");
	frameCount++;

	if (frameCount >= totalFrames) {
		ofExit(); 
	}

	// Batch script
	// Run this (on windows) with ffmpeg to generate a video using the frames 
	// https://ffmpeg.org/download.html
	// C:\ffmpeg-8.0-essentials_build\bin\ffmpeg.exe -framerate 30 -i out\output%05d.png -c:v libx264 -pix_fmt yuv420p out.mp4

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
