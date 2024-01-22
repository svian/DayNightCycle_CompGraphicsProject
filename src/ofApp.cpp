#include "ofApp.h"
#include "CameraMatrices.h"
#include "calcTangents.h"

//--------------------------------------------------------------
void ofApp::setup() {

    ofDisableArbTex();
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    ofEnableDepthTest();
    ofSetVerticalSync(false);

    using namespace glm;

    cycle.setup();

    cubeMesh.load("skybox/cube.ply");
    cubemap.load("skybox/skybox_front.png", "skybox/skybox_back.png", "skybox/skybox_right.png", "skybox/skybox_left.png", "skybox/skybox_top.png", "skybox/skybox_bottom.png");

    terrainMesh.load("terrain/source/flat_terrain.ply");
    calcTangents(terrainMesh);

#pragma region texures
    terrainTex.load("skybox/skybox_bottom.png");
    assert(terrainTex.getWidth() != 0 && terrainTex.getHeight() != 0);

    //generate mipmap
    terrainTex.getTexture().setTextureWrap(GL_REPEAT, GL_REPEAT);
    terrainTex.getTexture().generateMipmap();
    terrainTex.getTexture().setTextureMinMagFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    terrainNormal.load("terrain/textures/flat_normal.png");
    assert(terrainNormal.getWidth() != 0 && terrainNormal.getHeight() != 0);
    
    terrainNormal.getTexture().setTextureWrap(GL_REPEAT, GL_REPEAT);
    terrainNormal.getTexture().generateMipmap();
    terrainNormal.getTexture().setTextureMinMagFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
#pragma endregion


    treeMesh.load("tree/tree.ply");
    calcTangents(treeMesh);

    treeVbo.setMesh(treeMesh, GL_STATIC_DRAW);

#pragma region texures
    treeTex.load("tree/bark04.png");
    assert(treeTex.getWidth() != 0 && treeTex.getHeight() != 0);

    //generate mipmap
    treeTex.getTexture().setTextureWrap(GL_REPEAT, GL_REPEAT);
    treeTex.getTexture().generateMipmap();
    treeTex.getTexture().setTextureMinMagFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    treeNormal.load("tree/bark04_normal.png");
    assert(treeNormal.getWidth() != 0 && treeNormal.getHeight() != 0);

    treeNormal.getTexture().setTextureWrap(GL_REPEAT, GL_REPEAT);
    treeNormal.getTexture().generateMipmap();
    treeNormal.getTexture().setTextureMinMagFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
#pragma endregion

    reloadShaders();

    setTreePos();

    // Allocate a 1024x1024 RGB+alpha framebuffer
    ofFboSettings fboSettings{};
    fboSettings.width = 10240;
    fboSettings.height = 10240;
    fboSettings.internalformat = GL_RGBA;
    fboSettings.useDepth = true;
    fboSettings.depthStencilAsTexture = true;
    fbo.allocate(fboSettings);

}

//--------------------------------------------------------------
void ofApp::update() {
    if (needsReload)
    {
        reloadShaders();
    }

    using namespace glm;

    // Calculate world space velocity.
    vec3 velocityWorldSpace{ mat3(rotate(cameraHead, vec3(0, 1, 0))) * velocity };

    // Time since last frame
    float dt{ static_cast<float>(ofGetLastFrameTime()) };

    // Update position using velocity and dt.
    camera.position += velocityWorldSpace * dt;

    // Update rotation
    camera.rotation = rotate(-cameraHead, vec3(0, 1, 0)) *
        rotate(-cameraPitch, vec3(1, 0, 0));

    if (dt < 1.0f) {
        cycle.updateTime(dt);
    }

    //fpsFile.open("fps.txt", ios::app);
    //fpsFile << to_string(ofGetFrameRate()) + "," << endl;
    //fpsFile.close();

    //fpsFile.open("ms.txt", ios::app);
    //fpsFile << to_string(ofGetLastFrameTime() * 1000.0) + "," << endl;
    //fpsFile.close();


}
//--------------------------------------------------------------
void ofApp::draw() {
    using namespace glm;

    float width{ static_cast<float>(ofGetViewportWidth()) };
    float height{ static_cast<float>(ofGetViewportHeight()) };
    float aspect{ width / height };

    vec3 cycleShadows = vec3(clamp(cycle.sun.direction.x, -0.5f, 0.5f), clamp(cycle.sun.direction.y, -0.5f, 0.0f), 0) * cycle.sun.intensity;
    mat4 shadowView = mat4(mat3(lookAt(-cycleShadows, vec3(0), vec3(0, 1, 0))));
    mat4 shadowProj = ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f);
    CameraMatrices shadowCamMatrices{ shadowView, shadowProj };

    CameraMatrices camMatrices{ camera, aspect, 0.01f, 300.0f, };

    drawTrees(camMatrices, numOfTrees);
    drawShadows(camMatrices, shadowCamMatrices);
    drawFrametime();

}

//------------------------------------
void ofApp::reloadShaders()
{
    if (runningVertLight) { //calculations in vertex shader
        skyShader.load("shaders/optimized/opt_vertShader.vert", "shaders/skyboxFragShader.frag");
        lightShader.load("shaders/optimized/opt_vertShader.vert", "shaders/optimized/opt_lightFragShader.frag");
        shadowShd.load("shaders/optimized/opt_vertShader.vert", "shaders/optimized/opt_shadowShader.frag");
    }
    else {
        skyShader.load("shaders/vertShader.vert", "shaders/skyboxFragShader.frag");
        lightShader.load("shaders/vertShader.vert", "shaders/lightFragShader.frag");
        shadowShd.load("shaders/vertShader.vert", "shaders/shadowShader.frag");
    }
    needsReload = false;
}

void ofApp::drawTrees(const CameraMatrices& camM, float numOfTrees)
{
    using namespace glm;

    lightShader.begin();

    lightShader.setUniform3f("cameraPosition", camera.position);
    lightShader.setUniform3f("lightColor", cycle.sun.getColorIntensity());
    lightShader.setUniform3f("lightDir", cycle.sun.getDirectionToLight());
    lightShader.setUniform1f("lightIntensity", cycle.sun.intensity);

    lightShader.setUniformTexture("envMap", cubemap.getTexture(), 2);
    lightShader.setUniform3f("cameraPos", camera.position);
    lightShader.setUniformMatrix3f("normalMatrix", mat3());

    lightShader.setUniformTexture("diffuseTex", treeTex.getTexture(), 0);
    lightShader.setUniformTexture("normalTex", treeNormal.getTexture(), 1);

    for (int i = 0; i < numOfTrees; i++) {
        mat4 model = rotate(r[i], vec3(0,1,0)) * translate(vec3(x[i], 0.7f, z[i])) * scale(vec3(500.0f));
        mat4 mvp = camM.getProj() * camM.getView() * model;

        lightShader.setUniformMatrix4f("mvp", mvp);

        if (runningVbo) {
            treeVbo.drawElements(GL_TRIANGLES, treeVbo.getNumIndices());
        }
        else {
            treeMesh.draw();
        }
    }

    lightShader.end();

    glDisable(GL_CULL_FACE);
    drawSkybox(camM);
    glEnable(GL_CULL_FACE);
}

void ofApp::setTreePos()
{
    srand((unsigned)time(NULL));

    for (int i = 0; i < numOfTrees; ++i) {
        x[i] = -5 + (rand() % 11);
        z[i] = -6 + (rand() % 11);
        r[i] = (rand() % 11);
    }
}

void ofApp::drawSkybox(const CameraMatrices& camM)
{
    using namespace glm;

    mat4 model = translate(camM.getCamera().position) * scale(vec3(20));
    mat4 mvp = camM.getProj() * camM.getView() * model;

    ofShader& shd = skyShader;

    shd.setUniformMatrix4f("model", model);

    glDepthFunc(GL_LEQUAL);
    shd.begin();
    shd.setUniformMatrix4f("mvp", mvp);
    shd.setUniformTexture("cubemap", cubemap.getTexture(), 0);
    shd.setUniform3f("lightColor", cycle.sun.getColorIntensity());
    cubeMesh.draw();
    shd.end();
    glDepthFunc(GL_LESS);

}

void ofApp::drawFrametime()
{
    glDisable(GL_CULL_FACE);
    std::string frameTimeStr{
        to_string(ofGetLastFrameTime() * 1000.0) +
        "ms\n" + to_string(ofGetFrameRate()) + "fps" };

    ofDrawBitmapStringHighlight(frameTimeStr, 50, 50);
    glEnable(GL_CULL_FACE);
}

void ofApp::drawShadows(const CameraMatrices& camM, const CameraMatrices& camS)
{
    using namespace glm;

    fbo.begin();
    ofClear(0, 0, 0, 0);
    drawTrees(camS, numOfTrees);
    fbo.end();

    shadowShd.begin();
    shadowShd.setUniform3f("cameraPos", camM.getCamera().position);
    mat4 model = mat4(translate(vec3(0.0, -1.0, 0.0)) * scale(vec3(50.0f)));

    shadowShd.setUniformMatrix4f("model", model);
    shadowShd.setUniformMatrix4f("mvp", camM.getProj() * camM.getView() * model);
    shadowShd.setUniformMatrix3f("normalMatrix", mat3());
    shadowShd.setUniformMatrix4f("shadowMatrix", camS.getProj() * camS.getView());
    shadowShd.setUniformTexture("fboTex", fbo.getDepthTexture(), 0);
    shadowShd.setUniform1f("planeLight", cycle.sun.intensity);

    shadowShd.setUniform3f("cameraPosition", camera.position);
    shadowShd.setUniform3f("lightColor", cycle.sun.getColorIntensity());
    shadowShd.setUniform3f("lightDir", cycle.sun.getDirectionToLight());

    shadowShd.setUniformTexture("envMap", cubemap.getTexture(), 2);

    shadowShd.setUniformTexture("diffuseTex", terrainTex.getTexture(), 1);
    shadowShd.setUniformTexture("normalTex", terrainNormal.getTexture(), 2);
    shadowShd.setUniform1f("isShadows", 1);

    terrainMesh.draw();


    shadowShd.end();

}

void ofApp::updateCameraRotation(float dx, float dy)
{
    cameraHead += dx;
    cameraPitch += dy;

    using namespace glm;
    cameraPitch = clamp(cameraPitch, -static_cast<float>(PI) / 2, static_cast<float>(PI) / 2);
}

void ofApp::updateCameraRoll(float dx)
{
    cameraRoll += dx;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    float speed = 2;
    if (key == '`')
    {
        reloadShaders();
    }
    else if (key == 'w')
    {
        velocity.z = -speed;
    }
    else if (key == 's')
    {
        velocity.z = speed;
    }
    else if (key == 'a')
    {
        velocity.x = -speed;
    }
    else if (key == 'd')
    {
        velocity.x = speed;
    }
    else if (key == 'e')
    {
        velocity.y = speed;
    }
    else if (key == 'q')
    {
        velocity.y = -speed;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
    if (key == 'w' || key == 's')
    {
        velocity.z = 0;
    }
    else if (key == 'a' || key == 'd')
    {
        velocity.x = 0;
    }
    else if (key == 'e' || key == 'q')
    {
        velocity.y = 0;
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
    if (prevX != 0 && prevY != 0)
    {
        // Update camera rotation based on mouse movement
        updateCameraRotation(mouseSensitivity * (x - prevX), mouseSensitivity * (y - prevY));
    }

    // Remember where the mouse was this frame.
    prevX = x;
    prevY = y;
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
