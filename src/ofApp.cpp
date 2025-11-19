#include "ofApp.h"
#include <filesystem>

//--------------------------------------------------------------
void ofApp::setup() {
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
	world.push_back(std::make_shared<Sphere>(glm::vec3(0, 0, -1), 0.5f, glm::vec3(1.0f, 0.5f, 0.0f)));
	world.push_back(std::make_shared<Cylinder>(glm::vec3(2.0f, 2.0, -5.0f), 2.0f, 1.0f, glm::vec3(0.2f, 0.8f, 1.0f), glm::vec3(0.0, 0.0, 1.0)));
	world.push_back(std::make_shared<Cylinder>(glm::vec3(-3.0f, 3.0, -5.0f), 2.0f, 1.0f, glm::vec3(0.2f, 0.8f, 1.0f), glm::vec3(1.0, 0.0, 0.5)));

	// Create multiple lightningSegments
	for (int i = 0; i < 3; i++) {
		float x = ofRandom(-3.0f, 3.0f);
		float z = ofRandom(-6.0f, -2.0f);
		float y = ofRandom(3.0f, 6.0f);
		glm::vec3 pos(x, y, z);
		lightSources.emplace_back(pos);

		LightSource & ls = lightSources.back();
		auto lightningSegment = std::make_shared<LightningSegment>(pos, 0.05f, 2.0f, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), true, ls);
		lightningSegments.push_back(lightningSegment);
		world.push_back(lightningSegment);
	}

	pixels.allocate(screenWidth, screenHeight, OF_IMAGE_COLOR);
}

//--------------------------------------------------------------
void ofApp::update() {
	// Establish a unit of time for animations
	float frameTime = 1.0f / 30.0f; // Time per frame
	float elapsedTime = frameCount * frameTime; // Time since start
}

//--------------------------------------------------------------
void ofApp::draw() {

	for (int y = 0; y < screenHeight; y++) {
		for (int x = 0; x < screenWidth; x++) {
			glm::vec3 color = tracePixel(x, y, frameCount);
			color = glm::clamp(color, 0.0f, 1.0f);
			pixels.setColor(x, y, ofColor(color.r * 255, color.g * 255, color.b * 255));
		}
	}

	namespace fs = std::filesystem;
	fs::path cwd = fs::current_path();
	fs::path search = cwd;
	fs::path projectRoot;
	bool found = false;
	for (int i = 0; i < 10; ++i) {
		if (fs::exists(search / "src")) {
			projectRoot = search;
			found = true;
			break;
		}
		if (search.has_parent_path())
			search = search.parent_path();
		else
			break;
	}

	fs::path outPath = found ? (projectRoot / "out") : (cwd / "out");
	std::error_code ec;
	fs::create_directories(outPath, ec);

	std::string filename = "output" + ofToString(frameCount, 5, '0') + ".png";
	fs::path savePath = outPath / filename;

	ofLog() << "CWD: " << cwd.string();
	ofLog() << "Saving frame to (relative): " << (fs::relative(savePath, cwd)).string();
	ofLog() << "Saving frame to (absolute): " << fs::absolute(savePath).string();

	ofSaveImage(pixels, savePath.string());
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
    const int samplesPerPixel = 16;
    glm::vec3 accumulatedColor(0.0f);

    for (int s = 0; s < samplesPerPixel; ++s) {
		// jitter
        float u = (float(x) + ofRandom(0.0f, 1.0f)) / (screenWidth - 1);
        float v = (float(y) + ofRandom(0.0f, 1.0f)) / (screenHeight - 1);

        Ray r = cam.getRay(u, v);

        hit_record rec;
        float closest = 1e20f;
        glm::vec3 background(0.0f, 0.0f, 0.0f);
        glm::vec3 pixelColor = background;

        for (auto& obj : world) {
            if (obj->hit(r, 0.001f, closest, rec)) {
                closest = rec.t;
                glm::vec3 totalLightRGB(0.0f);
                for (auto& lightningSegment : lightningSegments) {
                    auto& light = *(lightningSegment->lightSource);
                    glm::vec3 L = light.position - rec.p;
                    float dist2 = glm::dot(L, L);
                    float dist = sqrt(dist2);
                    if (dist < 1e-4f) continue;
                    glm::vec3 lightDir = glm::normalize(L);
                    float diff = glm::max(glm::dot(rec.normal, lightDir), 0.0f);
                    totalLightRGB += glm::vec3(diff);
                }
                glm::vec3 ambient = 0.1f * rec.color;
                float brightness = glm::clamp(glm::max(glm::max(totalLightRGB.r, totalLightRGB.g), totalLightRGB.b), 0.0f, 1.0f);
                glm::vec3 washedColor = glm::mix(rec.color, glm::vec3(1.0f), brightness * 0.95f);
                glm::vec3 diffuse = washedColor * brightness;
                glm::vec3 emissive(0.0f);
                if (rec.color.r > 0.95f && rec.color.g > 0.95f && rec.color.b > 0.95f) {
                    float emissionFactor = glm::clamp(glm::max(glm::max(totalLightRGB.r, totalLightRGB.g), totalLightRGB.b), 0.0f, 10.0f);
                    emissive = rec.color * (emissionFactor * 0.35f);
                }
                pixelColor = ambient + diffuse + emissive;
                pixelColor = glm::clamp(pixelColor, glm::vec3(0.0f), glm::vec3(1.0f));
            }
        }
        accumulatedColor += pixelColor;
    }
    return accumulatedColor / float(samplesPerPixel);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {
}
