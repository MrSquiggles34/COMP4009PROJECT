#include "ofApp.h"
#include <filesystem>
#include <glm/gtc/random.hpp>



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
void ofApp::update() {
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
    (void)frame;

    const int SAMPLES_PER_LIGHT = 8;  // fewer samples for faster real-time, increase for smoother shadows
    const float EPS = 0.001f;

    float u = float(x) / (screenWidth - 1);
    float v = float(y) / (screenHeight - 1);

    Ray r = cam.getRay(u, v);

    hit_record rec;
    float closest = 1e20f;

    glm::vec3 background(0.0f);
    glm::vec3 pixelColor = background;

    // 1. OBJECT INTERSECTION 
    bool hitAnything = false;
    for (auto& obj : world) {
        if (obj->hit(r, EPS, closest, rec)) {
            closest = rec.t;
            hitAnything = true;
        }
    }

    if (hitAnything) {
        // Immediate return if the object is emissive
        if (rec.emissive) {
            return glm::clamp(rec.emissionColor, 0.0f, 1.0f);
        }

        glm::vec3 totalLightRGB(0.0f);

        for (auto& lightningSegment : lightningSegments) {
            if (!lightningSegment->isEmissive()) continue;
            auto& light = *(lightningSegment->lightSource);

            glm::vec3 totalSampleColor(0.0f);

            glm::vec3 segStart = lightningSegment->startPoint;
            glm::vec3 segVec = lightningSegment->endPoint - lightningSegment->startPoint;

            for (int s = 0; s < SAMPLES_PER_LIGHT; s++) {
                float tSample = glm::linearRand(0.0f, 1.0f);
                glm::vec3 samplePos = segStart + tSample * segVec;

            
                if (light.radius > 0.0f) {
                    glm::vec3 jitter = glm::sphericalRand(light.radius * 0.5f);
                    samplePos += jitter;
                }

                glm::vec3 L = samplePos - rec.p;
                float dist2 = glm::dot(L, L);
                float dist = sqrt(dist2);
                if (dist <= 0.0f) continue;
                glm::vec3 lightDir = L / dist;

                
                if (glm::dot(lightDir, rec.normal) <= 0.0f) continue;

                Ray shadow(rec.p + rec.normal * EPS, lightDir);

                bool inShadow = false;
                hit_record shadowRec;

                
                for (auto& obj2 : world) {
                    if (obj2->hit(shadow, EPS, dist - EPS, shadowRec)) {
                        inShadow = true;
                        break;
                    }
                }
                if (inShadow) continue;

                
                for (auto& otherSeg : lightningSegments) {
                    if (otherSeg.get() == lightningSegment.get()) continue;
                    if (otherSeg->hit(shadow, EPS, dist - EPS, shadowRec)) {
                        inShadow = true;
                        break;
                    }
                }
                if (inShadow) continue;

                // Lambertian shading
                float nDotL = glm::max(glm::dot(rec.normal, lightDir), 0.0f);
                float attenuation = light.intensity / (dist2 + 1e-4f);

                totalSampleColor += (light.color * attenuation) * nDotL;
            }

            totalSampleColor /= float(SAMPLES_PER_LIGHT);
            totalLightRGB += totalSampleColor;
        }

        glm::vec3 ambient = 0.05f * rec.color;  // reduced ambient to make shadows visible
        glm::vec3 diffuse = rec.color * totalLightRGB;
        pixelColor = glm::clamp(ambient + diffuse, 0.0f, 1.0f);
    }

    // 2. LIGHTNING BOLT HIT TEST
    for (auto& seg : lightningSegments) {
        hit_record lrec;
        if (seg->hit(r, EPS, closest, lrec)) {
            if (seg->isEmissive() && seg->lightSource) {
                glm::vec3 emitted = seg->lightSource->color * seg->lightSource->intensity;
                return glm::clamp(emitted, 0.0f, 1.0f);
            } else {
                return glm::clamp(lrec.color, 0.0f, 1.0f);
            }
        }
    }

    return pixelColor;
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
