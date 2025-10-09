#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	// Load resources
	basic.load("../../src/basic.vert", "../../src/basic.frag");

	// Miscellanous 
	ofSetVerticalSync(true);
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
