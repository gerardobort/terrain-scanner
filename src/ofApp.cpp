#include "ofApp.h"
#include <math.h>
#include <ctype.h>

//--------------------------------------------------------------
void ofApp::setup(){

    ofSetFrameRate(60);

    ofSetWindowTitle("Terrain Scanner - Prototype");

    imageSource.loadImage("images/IMG_6864.JPG");
    //imageSource.loadImage("images/test.jpg");
    bufferPreCanvas.allocate(imageSource.getWidth(), imageSource.getHeight(), GL_RGB);
    bufferCanvas.allocate(imageSource.getWidth(), imageSource.getHeight(), GL_RGB);
    bufferCanvas.getTextureReference().getTextureData().bFlipTexture = true;
    bufferTerrainColor.allocate(4, 4, GL_RGBA);
    shaderMain.load("shaders/mapping");
    shaderMain.begin();
        shaderMain.setUniformTexture("u_sampler2d", imageSource.getTextureReference(), imageSource.getTextureReference().getTextureData().textureID);
    shaderMain.end();


    //finder.setup("haar/haarcascade_frontalface_default.xml");
    //finder.setup("haar/sqcows/classifier/cascade.xml");
    finder.setup("haar/haarcascade_upperbody.xml");
    
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
    shaderMain.end();
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
    bufferCanvas.end();
    bufferCanvas.draw(dx, dy);
    imageSource.draw(10, 10, imageSource.getWidth()/20.0, imageSource.getHeight()/20.0);

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
}

//--------------------------------------------------------------
void ofApp::setupGui() {
	gui.setup("Terrain Scanner", "settings.xml", ofGetWidth() - 210, 10);
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
    gui.add(parametersFilter);

	gui.loadFromFile("settings.xml");
	gui.minimizeAll();
}

void ofApp::mouseScrolled(double x, double y){
    ofLog(OF_LOG_NOTICE, "scroll X:" + ofToString(x));
    ofLog(OF_LOG_NOTICE, "scroll Y:" + ofToString(y));
}

