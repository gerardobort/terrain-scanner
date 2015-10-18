#pragma once

#include "ofMain.h"
#include "ofGLProgrammableRenderer.h"
#include "ofxGui.h"
#include "ofxCvHaarFinder.h"
#include "ofCowClassifier.h"

#define M_PI 3.1415926535897932384626433832795
#define USE_PROGRAMMABLE_GL
#define INT_COW_CLASSIFIERS_AMOUNT 6

class ofApp : public ofBaseApp{

    public:
        void setup();
        void update();
        void draw();
        void exit();
		void mouseDragged(int x, int y, int button);
        void mousePressed(int x, int y, int button);
        void mouseMoved(int x, int y);
        void keyPressed(int key);

        void drawCanvas();
        void mouseScrolled(int x, int y);

        stringstream streamReport;

        ofMesh meshCanvas;
        ofImage imageSource;
        ofFbo bufferPreCanvas;
        ofFbo bufferCanvas;
        ofFbo bufferTerrainColor;
        ofShader shaderMain;

        // GUI
        ofxPanel			gui;
        void				setupGui();
	    ofParameterGroup	parametersCanvas;
        ofParameter<int>	dx, dy;

	    ofParameterGroup	parametersFilter;
        ofParameter<bool>   enableFilter;
        ofParameter<float>  maskFactor;
        ofParameter<int>    maskUsePow;
        ofParameter<ofColor> terrainAvgColor;

        ofxCvHaarFinder     finder;

        ofCowClassifier     *cowClassifiers[INT_COW_CLASSIFIERS_AMOUNT];
};
