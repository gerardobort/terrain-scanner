#include "ofCowClassifier.h"
#include <math.h>

using namespace ofxCv;
using namespace cv;

//--------------------------------------------------------------
ofCowClassifier::ofCowClassifier(char *filterName, ofImage *imageSource)
    : filterName(filterName), imageSource(*imageSource) {

    changed = false;
    font.loadFont("fonts/Vera.ttf", 8);
    bufferOutput.allocate((*imageSource).getWidth(), (*imageSource).getHeight(), GL_RGBA);

    setupGui();
}

//--------------------------------------------------------------
ofCowClassifier::~ofCowClassifier(){
}

//--------------------------------------------------------------
void ofCowClassifier::update(){

    if (!enabled) return;
    if (!changed) return;
    ofLog(OF_LOG_NOTICE, "**** UPDATE!");

    contourFinder.setMinAreaRadius(minRadius);
    contourFinder.setMaxAreaRadius(maxRadius);
    trackingColorMode = TRACK_COLOR_RGB;

    contourFinder.setThreshold(cannyThreshold);
    contourFinder.setFindHoles(true);
    contourFinder.setTargetColor(targetColor);
    contourFinder.findContours(imageSource);
}

//--------------------------------------------------------------
void ofCowClassifier::draw(){

    if (!enabled) return;

    if (changed) {
    ofLog(OF_LOG_NOTICE, "**** CHANGED!");
    bufferOutput.begin();
        ofClear(0);
        ofSetLineWidth(2);
        ofSetColor(255, 255, 255, 30);
        contourFinder.draw();
        
        int n = contourFinder.size();
        for(int i = 0; i < n; i++) {
            // balance is useful for detecting when a shape has an "arm" sticking out
            // if balance.length() is small, the shape is more symmetric: like I, O, X...
            // if balance.length() is large, the shape is less symmetric: like L, P, F...
            ofVec2f balance = toOf(contourFinder.getBalance(i));

            if (balance.length() > balanceThreshold) continue;

            // smallest rectangle that fits the contour
            ofSetColor(cyanPrint);
            ofPolyline minAreRect = toOf(contourFinder.getMinAreaRect(i));
            minAreRect.draw();
            
            // ellipse that best fits the contour
            ofSetColor(magentaPrint);
            cv::RotatedRect ellipse = contourFinder.getFitEllipse(i);
            ofPushMatrix();
            ofVec2f ellipseCenter = toOf(ellipse.center);
            ofVec2f ellipseSize = toOf(ellipse.size);
            ofTranslate(ellipseCenter.x, ellipseCenter.y);
            ofRotate(ellipse.angle);
            ofEllipse(0, 0, ellipseSize.x, ellipseSize.y);
            ofPopMatrix();

            // convex hull of the contour
            ofSetColor(yellowPrint);
            ofPolyline convexHull = toOf(contourFinder.getConvexHull(i));
            if (fillPolyline) {
                ofPushStyle();
                ofFill();
                ofSetColor(ofColor(200, 0, 0, 60));
                ofBeginShape();
                vector<ofPoint>& vertices = convexHull.getVertices();
                for(int j = 0; j < vertices.size(); j++) {
                    ofVertex(vertices[j]);
                }
                ofEndShape();
                ofNoFill();
                ofPopStyle();
            }
            convexHull.draw();
            
            // minimum area circle that encloses the contour
            if (i == collisionContourIndex) {
                ofPushStyle();
                ofSetColor(255);
                ofSetColor(cyanPrint);
                float circleRadius;
                ofVec2f circleCenter = toOf(contourFinder.getMinEnclosingCircle(i, circleRadius));

                // you can also get the area and perimeter using ofPolyline:
                // ofPolyline::getArea() and ofPolyline::getPerimeter()
                double area = contourFinder.getContourArea(i);
                double length = contourFinder.getArcLength(i);      

                /*
                ofFill();
                ofSetColor(ofColor(200, 0, 0, 120));
                ofCircle(circleCenter, circleRadius);
                ofNoFill();
                */
                ofFill();
                ofSetColor(ofColor(0, 255, 0, 100));
                ofBeginShape();
                vector<ofPoint>& vertices = convexHull.getVertices();
                for(int j = 0; j < vertices.size(); j++) {
                    ofVertex(vertices[j]);
                }
                ofEndShape();
                ofNoFill();

                ofPopStyle();

                ofSetColor(0, 255, 0, 255);
                font.drawString(
                    "from: " + ofToString(filterName)
                    + "\narea: " + ofToString(area)
                    + "\nlength: " + ofToString(length)
                    + "\nbalance: " + ofToString(balance.length())
                    + "\ncows: " + ofToString(ceil(area/estimatedCowArea)),  // cow estimated area
                    circleCenter.x + circleRadius + 10, circleCenter.y
                );
            }
            
            // defects of the convex hull
            vector<cv::Vec4i> defects = contourFinder.getConvexityDefects(i);
            for(int j = 0; j < defects.size(); j++) {
                ofLine(defects[j][0], defects[j][1], defects[j][2], defects[j][3]);
            }
            
            // some different styles of contour centers
            ofVec2f centroid = toOf(contourFinder.getCentroid(i));
            ofVec2f average = toOf(contourFinder.getAverage(i));
            ofVec2f center = toOf(contourFinder.getCenter(i));
            ofSetColor(cyanPrint);
            ofCircle(centroid, 1);
            ofSetColor(magentaPrint);
            ofCircle(average, 1);
            ofSetColor(yellowPrint);
            ofCircle(center, 1);
            
            ofPushMatrix();
            ofTranslate(centroid.x, centroid.y);
            ofScale(5, 5);
            ofLine(0, 0, balance.x, balance.y);
            ofPopMatrix();
        }

        ofSetColor(255);
    bufferOutput.end();

    changed = false;
    }
    bufferOutput.draw(0, 0);
}

//--------------------------------------------------------------
void ofCowClassifier::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofCowClassifier::mousePressed(int x, int y, int button) {
    targetColor = imageSource.getPixelsRef().getColor(x, y);
    contourFinder.setTargetColor(targetColor, trackingColorMode);
}

//--------------------------------------------------------------
void ofCowClassifier::mouseMoved(int x, int y){
    int n = contourFinder.size();
    for(int i = 0; i < n; i++) {
        float circleRadius;
        ofVec2f circleCenter = toOf(contourFinder.getMinEnclosingCircle(i, circleRadius));
        if (circleCenter.distance(ofVec2f(x, y)) < circleRadius) {
            collisionContourIndex = i;
            ofLog(OF_LOG_NOTICE, "COLLISION! " + ofToString(i));
            changed = true;
        }
    }
}

//--------------------------------------------------------------
void ofCowClassifier::keyPressed(int key){

    if (!enabled) return;

    switch (key) {
        case 'h':
            trackingColorMode = TRACK_COLOR_HS;
            break;
        case 'r':
            trackingColorMode = TRACK_COLOR_RGB;
            break;
    }
    contourFinder.setTargetColor(targetColor, trackingColorMode);
}

void ofCowClassifier::setupGui() {
    char name[100];
    sprintf(name, "CowClassifier_%s", filterName);
	parameters.setName(name);

	parametersContour.setName("Contour_Finder");
	parametersContour.add(enabled.set("enabled", false));
	parametersContour.add(cannyThreshold.set("cannyThreshold", 200, 0, 255));
	parametersContour.add(minRadius.set("minRadius", 5, 5, 300));
	parametersContour.add(maxRadius.set("maxRadius", 30, 5, 300));
	parametersContour.add(fillPolyline.set("fillPolyline", false));
	parametersContour.add(targetColor.set("targetColor", ofColor(0, 100, 0), ofColor(0, 0, 0), ofColor(255, 255, 255)));
	parametersContour.add(balanceThreshold.set("balanceThreshold", 6, 0, 50));
	parametersContour.add(estimatedCowArea.set("estimatedCowArea", 200, 1, 400));

	enabled.addListener(this, &ofCowClassifier::setEnabled);
	cannyThreshold.addListener(this, &ofCowClassifier::setCannyThreshold);
	minRadius.addListener(this, &ofCowClassifier::setMinRadius);
	maxRadius.addListener(this, &ofCowClassifier::setMaxRadius);
	targetColor.addListener(this, &ofCowClassifier::setTargetColor);
	parameters.add(parametersContour);
}
