#include "ofApp.h"
#include <math.h>
#include <ctype.h>

//--------------------------------------------------------------
void ofApp::setup(){

    ofSetFrameRate(60);

    ofSetWindowTitle("Terrain Scanner - Prototype");

    imageSource.loadImage("images/IMG_6864.JPG");
    //imageSource.loadImage("images/terrain.png");
    //imageSource.loadImage("images/test.jpg");
    bufferPreCanvas.allocate(imageSource.getWidth(), imageSource.getHeight(), GL_RGB);
    bufferCanvas.allocate(imageSource.getWidth(), imageSource.getHeight(), GL_RGB);
    bufferCanvas.getTextureReference().getTextureData().bFlipTexture = true;
    bufferTerrainColor.allocate(4, 4, GL_RGBA);
    shaderMain.load("shaders/mapping");
    shaderMain.begin();
        shaderMain.setUniformTexture("u_sampler2d", imageSource.getTextureReference(), imageSource.getTextureReference().getTextureData().textureID);
    shaderMain.end();


    //finder.setup("haar/sqcows/classi.xml");
    finder.setup("haar/haarcascade_frontalface_default.xml");

    cowClassifiers[0] = new ofCowClassifier("Shorthorn", &imageSource);
    cowClassifiers[1] = new ofCowClassifier("Hereford", &imageSource);
    cowClassifiers[2] = new ofCowClassifier("Aberdeen_Angus", &imageSource);
    cowClassifiers[3] = new ofCowClassifier("Holando_Argentina", &imageSource);
    cowClassifiers[4] = new ofCowClassifier("Zebu", &imageSource);
    cowClassifiers[5] = new ofCowClassifier("Charolais", &imageSource);
    
    dx = dy = 0;
	setupGui();
}

//--------------------------------------------------------------
void ofApp::update(){
    //int w = ofGetWidth(), h = ofGetHeight(), d = 0;
    int w = imageSource.getWidth(), h = imageSource.getHeight(), d = 0;

    meshCanvas.clear();
    meshCanvas.addVertex(ofVec3f(0, 0, d));
    meshCanvas.addVertex(ofVec3f(w, 0, d));
    meshCanvas.addVertex(ofVec3f(0, h, d));
    meshCanvas.addVertex(ofVec3f(w, 0, d));
    meshCanvas.addVertex(ofVec3f(0, h, d));
    meshCanvas.addVertex(ofVec3f(w, h, d));

    meshCanvas.addColor(ofColor(0, 100, 0));
    meshCanvas.addColor(ofColor(0, 100, 0));
    meshCanvas.addColor(ofColor(0, 100, 0));
    meshCanvas.addColor(ofColor(0, 100, 0));
    meshCanvas.addColor(ofColor(0, 100, 0));
    meshCanvas.addColor(ofColor(0, 100, 0));

    shaderMain.begin();
        shaderMain.setUniform1f("u_maskFactor", maskFactor);
        shaderMain.setUniform1i("u_maskUsePow", maskUsePow);
        shaderMain.setUniform1i("u_enableFilter", enableFilter ? 1 : 0);
        shaderMain.setUniform3f("u_terrainAvgColor", terrainAvgColor->r/255.0, terrainAvgColor->g/255.0, terrainAvgColor->b/255.0);
    shaderMain.end();

    for (int i= 0 ; i < INT_COW_CLASSIFIERS_AMOUNT; i++)
        cowClassifiers[i]->update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofClear(0);
    ofEnableAlphaBlending();
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);

    ofColor centerColor(100, 100, 100);
    ofColor edgeColor(40, 40, 40);
    ofBackgroundGradient(centerColor, edgeColor, OF_GRADIENT_CIRCULAR);

    bufferCanvas.begin();
        drawCanvas();

        // haar feature points
        ofNoFill();
        for(int i = 0; i < finder.blobs.size(); i++) {
            ofRectangle cur = finder.blobs[i].boundingRect;
            ofRect(cur.x, cur.y, cur.width, cur.height);
        }


        for (int i= 0 ; i < INT_COW_CLASSIFIERS_AMOUNT; i++)
            cowClassifiers[i]->draw();

    bufferCanvas.end();
    bufferCanvas.draw(dx, dy);

    bufferCanvas.draw(10, 10, imageSource.getWidth()/15.0, imageSource.getHeight()/15.0);
    //imageSource.draw(10, 10, imageSource.getWidth()/20.0, imageSource.getHeight()/20.0);

    gui.draw();

    ofEnableBlendMode(OF_BLENDMODE_DISABLED);
    ofDisableAlphaBlending();
}

void ofApp::drawCanvas(){
    bufferPreCanvas.begin();
        shaderMain.begin();
            ofSetColor(255, 255, 255, 255);
            ofPushMatrix();
                meshCanvas.drawFaces();
            ofPopMatrix();
        shaderMain.end();
    bufferPreCanvas.end();
    bufferPreCanvas.draw(0, 0);
}


//--------------------------------------------------------------
void ofApp::exit(){
}


//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    ofLog(OF_LOG_NOTICE, "drag X:" + ofToString(x));
    ofLog(OF_LOG_NOTICE, "drag Y:" + ofToString(y));
    ofLog(OF_LOG_NOTICE, "drag button:" + ofToString(button));
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    // OF_KEY_BACKSPACE, OF_KEY_RETURN, OF_KEY_PRINTSCR, OF_KEY_F1 - OF_KEY_F12, OF_KEY_LEFT, OF_KEY_UP, OF_KEY_RIGHT, OF_KEY_DOWN, OF_KEY_PAGE_UP, OF_KEY_PAGE_DOWN, OF_KEY_HOME, OF_KEY_END, OF_KEY_INSERT
    switch (key) {
        case OF_KEY_UP:
            if (dy < 0)
                dy+=30;
            break;
        case OF_KEY_DOWN:
            if (dy + imageSource.getHeight() > ofGetHeight())
                dy-=30;
            break;
        case OF_KEY_LEFT:
            if (dx < 0)
                dx+=30;
            break;
        case OF_KEY_RIGHT:
            if (dx + imageSource.getWidth() > ofGetWidth())
                dx-=30;
            break;
        case OF_KEY_RETURN:
            ofPixels tmpPixels;
            ofImage tmpImage;
            bufferPreCanvas.readToPixels(tmpPixels);
            tmpImage.setFromPixels(tmpPixels);
            finder.findHaarObjects(tmpImage);
            break;
    }
    for (int i= 0 ; i < INT_COW_CLASSIFIERS_AMOUNT; i++)
        cowClassifiers[i]->keyPressed(key);
}

//--------------------------------------------------------------
void ofApp::setupGui() {
	gui.setup("Terrain Scanner", "settings.xml", ofGetWidth() - 260, 10);
	gui.setDefaultBackgroundColor(ofColor(0, 0, 0, 127));
	gui.setDefaultFillColor(ofColor(160, 160, 160, 160));

	parametersCanvas.setName("Navigation");
	parametersCanvas.add(dx.set("dx", 0, -imageSource.getWidth() + ofGetWidth(), 0));
	parametersCanvas.add(dy.set("dy", 0, -imageSource.getHeight() + ofGetHeight(), 0));
    gui.add(parametersCanvas);

	parametersFilter.setName("Grass filter");
	parametersFilter.add(enableFilter.set("enableFilter", true));
	parametersFilter.add(maskFactor.set("maskFactor", 1.0, 0, 30.0));
	parametersFilter.add(maskUsePow.set("maskUsePow", 1, 1, 6));
	parametersFilter.add(terrainAvgColor.set("terrainAvgColor", ofColor(200, 200, 200), ofColor(0, 0, 0), ofColor(255, 255, 255)));
    gui.add(parametersFilter);

    for (int i= 0 ; i < INT_COW_CLASSIFIERS_AMOUNT; i++)
        gui.add(cowClassifiers[i]->parameters);

	gui.loadFromFile("settings.xml");
	gui.minimizeAll();
}

void ofApp::mouseScrolled(int x, int y){
    ofLog(OF_LOG_NOTICE, "scroll X:" + ofToString(x));
    ofLog(OF_LOG_NOTICE, "scroll Y:" + ofToString(y));
}

void ofApp::mousePressed(int x, int y, int button) {
    ofLog(OF_LOG_NOTICE, "press X:" + ofToString(x));
    ofLog(OF_LOG_NOTICE, "press Y:" + ofToString(y));
    ofLog(OF_LOG_NOTICE, "press button:" + ofToString(button));
}

void ofApp::mouseMoved(int x, int y){
    for (int i= 0 ; i < INT_COW_CLASSIFIERS_AMOUNT; i++)
        cowClassifiers[i]->mouseMoved(x -dx, y -dy);
}
