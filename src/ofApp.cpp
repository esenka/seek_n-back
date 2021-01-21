#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    
    seek.setup(OFX_SEEK_THERMAL_CAM_COMPACT);
    seek.setCVColorMap(cv::COLORMAP_BONE);
    seek.setVerbose(false);
    _img.allocate(THERMAL_WIDTH, THERMAL_HEIGHT, OF_IMAGE_COLOR);
//    ofAddListener(seek.new_frame_evt, this, &ofApp::seekFrameCallback);
    
    nback.setup(3, 500, 2000, 0.2, ESEN_FONT_PATH);
}

//--------------------------------------------------------------
void ofApp::update(){
    if(seek.isInitialized()){
        if(seek.isFrameNew()) _img.setFromPixels(seek.getVisualizePixels());
    }
    if(nback.isTestRunning()){
        nback.update();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    nback.draw(512,384,400,400);
    _img.draw(50,50);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    switch(key){
        case OF_KEY_LEFT_SHIFT:
            break;
        case OF_KEY_RIGHT_SHIFT:
            break;
        default:
            // no need to do nothing but put this just in case...
            break;
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

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

//--------------------------------------------------------------
void ofApp::exit(){
#ifdef VERBOSE_MODE
    cout << __PRETTY_FUNCTION__ << endl;
#endif
    if(nback.isTestRunning()){
        nback.stop();
    }
    seek.close();
    
//    ofRemoveListener(seek.new_frame_evt, this, &ofApp::seekFrameCallback);
    this->exitGui();
}

//--------------------------------------------------------------
void ofApp::seekFrameCallback(bool &val){
    if(seek.isInitialized()){
        _img.setFromPixels(seek.getVisualizePixels());
        if(_is_recording){
            cv::Mat buf;
            seek.getRawCVFrame(buf);
            ofxSeekThermal::writepgm(buf, _recording_path + "/" + _file_prefix.get(),
                                     _seek_frame_number, "", false);
            _seek_frame_number++;
        }
    }
}


void ofApp::setupGui(){
    parameters.setName("parameters");
    parameters.add(_nback_running.set("Nback Run/Stop", false));
    parameters.add(_file_path.set("Recording Path", "seek_frame_samples"));
    parameters.add(_file_prefix.set("File prefix", "frame_"));
    parameters.add(_recording.set("Recording", false));
    parameters.add(_kill_app.set("Quit App?", false));
    
    _nback_running.addListener(this, &ofApp::nbackCallback);
    _recording.addListener(this, &ofApp::recordingCallback);
    _kill_app.addListener(this, &ofApp::killAppCallback);
    
    gui.setup(parameters);
}

void ofApp::drawGui(ofEventArgs &args){
    gui.draw();
}

void ofApp::exitGui(){
    _nback_running.removeListener(this, &ofApp::nbackCallback);
    _recording.removeListener(this, &ofApp::recordingCallback);
    _kill_app.removeListener(this, &ofApp::killAppCallback);
}

void ofApp::nbackCallback(bool & val){
#ifdef VERBOSE_MODE
    cout << __PRETTY_FUNCTION__ << val << endl;
#endif
    if(val){
        nback.start();
    }else{
        nback.stop();
    }
}

void ofApp::recordingCallback(bool & val){
#ifdef VERBOSE_MODE
    cout << __PRETTY_FUNCTION__ << val << endl;
#endif
    if(val){
        if(!ofFilePath::isAbsolute(_file_path.get())){
            _recording_path = ofFilePath::getAbsolutePath(_file_path.get());
        }else{
            _recording_path = _file_path.get();
        }
        if(!ofDirectory::doesDirectoryExist(_recording_path)){
            ofDirectory::createDirectory(_recording_path);
        }else{
            // things
        }
        _seek_frame_number = 0;
        _is_recording = true;
    }else{
        _is_recording = false;
    }
}

void ofApp::killAppCallback(bool & val){
    if(val){
        ofExit();
    }
}
