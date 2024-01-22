#include "ofMain.h"
#include "ofApp.h"
#include <GLFW/glfw3.h>

//========================================================================
int main() {
	ofGLWindowSettings glSettings;
	glSettings.setSize(1024, 768);
	glSettings.windowMode = OF_WINDOW;
	glSettings.setGLVersion(4, 1);
	ofCreateWindow(glSettings);

	auto window{ ofGetCurrentWindow() };
	glfwSetInputMode(
		dynamic_pointer_cast<ofAppGLFWWindow>(window)->getGLFWWindow(),
		GLFW_CURSOR, GLFW_CURSOR_DISABLED
	);

	ofRunApp(new ofApp());

}
