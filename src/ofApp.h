#pragma once

#include "ofMain.h"
#include "ofGLProgrammableRenderer.h"
#include "ofxGui.h"
#include "ofxCvHaarFinder.h"

#define M_PI 3.1415926535897932384626433832795
#define USE_PROGRAMMABLE_GL

class ofApp : public ofBaseApp{

    public:
        void setup();
        void update();
        void draw();
        void exit();
		void mouseDragged(int x, int y, int button);
        void keyPressed(int key);

        void drawCanvas();
        void mouseScrolled(double x, double y);

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

        ofxCvHaarFinder     finder;
};
