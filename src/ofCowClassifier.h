#pragma once

#include <math.h>
#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"

#define M_PI 3.1415926535897932384626433832795

class ofCowClassifier {

    public:
        ofCowClassifier(char *filterName, ofImage *imageSource);
        virtual ~ofCowClassifier();
        void update();
        void draw();
		void mouseDragged(int x, int y, int button);
        void mousePressed(int x, int y, int button);
        void keyPressed(int key);
        char *filterName;
        bool changed;

        ofImage imageSource;

        // GUI
        void				setupGui();
	    ofParameterGroup	parameters;
	    ofParameterGroup	parametersContour;
        ofParameter<bool>   enabled;
	    void setEnabled(bool& _value) { changed = true; }
        ofParameter<float>  cannyThreshold;
	    void setCannyThreshold(float& _value) { changed = true; }
        ofParameter<int>    minRadius;
	    void setMinRadius(int& _value) { changed = true; }
        ofParameter<int>    maxRadius;
	    void setMaxRadius(int& _value) { changed = true; }
        ofParameter<ofColor> targetColor;
	    void setTargetColor(ofColor& _value) { changed = true; }


        ofxCv::ContourFinder contourFinder;
        ofxCv::TrackingColorMode trackingColorMode;

};
