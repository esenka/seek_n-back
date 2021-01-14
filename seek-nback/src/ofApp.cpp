#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    
    seek.setup(OFX_SEEK_THERMAL_CAM_COMPACT);
    seek.setCVColorMap(cv::COLORMAP_BONE);
    seek.setVerbose(false);
    img.allocate(THERMAL_WIDTH, THERMAL_HEIGHT, OF_IMAGE_COLOR);
    
    nback.setup(3, 500, 2000, 0.2, "myApps/seek-nback/bin/data/gt-pressura-regular.ttf"
               );
    
    nback.start();
    

}

//--------------------------------------------------------------
void ofApp::update(){
    if(seek.isInitialized()){
        img.setFromPixels(seek.getVisualizePixels());
    }
    if(nback.isTestRunning()){
        nback.update();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    nback.draw(512,384,400,400);
    img.draw(50,50);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
