#pragma once

#include "ofMain.h"
#include "Camera.h"
#include "ofxCubemap.h"
#include "CameraMatrices.h"
#include "DirectionalLight.h"
#include "DayNightCycle.h"
#include <iostream>

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	ofShader skyShader;
	ofShader lightShader;
	ofShader shadowShd;

	//skybox
	ofMesh cubeMesh;
	ofxCubemap cubemap;

	//terrain
	ofMesh terrainMesh;
	ofImage terrainTex;
	ofImage terrainNormal;

	//tree
	ofMesh treeMesh;
	ofVbo treeVbo;
	ofImage treeTex;
	ofImage treeNormal;

	ofFbo fbo{};
	DayNightCycle cycle;

private:
	bool needsReload{ true };
	void reloadShaders();

	void drawTrees(const CameraMatrices& camM, float numOfTrees);
	void setTreePos();
	void drawSkybox(const CameraMatrices& camM);
	void drawFrametime();
	void drawShadows(const CameraMatrices& camM, const CameraMatrices& camS);

	static const int numOfTrees{ 80 };//number of trees generated on plane

	float x[numOfTrees];
	float z[numOfTrees];
	float r[numOfTrees];

	//Optimization:
	//Poorest: ruuningVertLight = true, running Vbo = false
	//Best: ruuningVertLight = false, running Vbo = true
	bool runningVertLight{ false }; //run the lighting calculations in the vertex shader
	bool runningVbo{ true }; //use vbo to generate trees

	//to track fps rate and averages
	ofstream fpsFile;

	// Position of the camera in world space
	Camera camera{ glm::vec3(0, 0, 2) };

	DirectionalLight dirLight;

	// (x, y) from the previous frame
	int prevX{ 0 }, prevY{ 0 };

	// How many radians of rotation correspond to a single pixel of movement of the cursor.
	float mouseSensitivity{ 0.02f };

	// The current head direction of the camera in radians.
	float cameraHead{ 0 };

	// The current pitch angle of the camera in radians.
	float cameraPitch{ 0 };

	// The current roll angle of the camera in radians.
	float cameraRoll{ 0 };

	// Velocity of the camera (from WASD) -- in camera space
	glm::vec3 velocity{ };

	// update camera rotation based on mouse movement
	void updateCameraRotation(float dx, float dy);

	// update camera rotation based on mouse movement
	void updateCameraRoll(float dx);
};
