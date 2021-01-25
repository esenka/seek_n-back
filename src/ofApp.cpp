#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    
    seek.setup(OFX_SEEK_THERMAL_CAM_COMPACT);
    seek.setCVColorMap(cv::COLORMAP_BONE);
    seek.setVerbose(false);
    _img.allocate(THERMAL_WIDTH, THERMAL_HEIGHT, OF_IMAGE_COLOR);
    ofAddListener(seek.new_frame_evt, this, &ofApp::seekFrameCallback);
    
    nback.setup(3, 500, 2000, 0.2, ESEN_FONT_PATH);
    ofAddListener(nback.new_char_evt, this, &ofApp::nbackNewCharCallback);
    ofAddListener(nback.char_hidden_evt, this, &ofApp::nbackCharHiddenCallback);
    
    _csv_header.push_back("system_timestamp_millis");
    _csv_header.push_back("system_status");     // START, STOP
    _csv_header.push_back("frame_file_name");
    _csv_header.push_back("nback_state");       // START, STOP, NEW, HIDDEN
    _csv_header.push_back("nback_character");   // current character
    _csv_header.push_back("nback_response");    // subject's response
    _csv_header.push_back("nback_true_or_false");   // T/F
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
    nback.draw(512, 384, 400, 400);
    _img.draw(50, 50);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    switch(key){
        case OF_KEY_LEFT_SHIFT:     // TRUE
            if(!nback.isResponseSubmitted()){
                nback.submitResponse(true);
                csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                        "-", "-", "-",
                                        nback.getLastCharacter(), "TRUE",
                                        this->nbackResultToString(nback.isLastResponseTrue()));
            }
            break;
        case OF_KEY_RIGHT_SHIFT:    // FALSE
            if(!nback.isResponseSubmitted()){
                nback.submitResponse(false);
                csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                        "-", "-", "-",
                                        nback.getLastCharacter(), "FALSE",
                                        this->nbackResultToString(nback.isLastResponseTrue()));
            }
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
        csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                "ERROR", "-", "-", "-", "-", "-");
        nback.stop();
    }
    seek.close();
    
    ofRemoveListener(seek.new_frame_evt, this, &ofApp::seekFrameCallback);
    ofRemoveListener(nback.new_char_evt, this, &ofApp::nbackNewCharCallback);
    ofRemoveListener(nback.char_hidden_evt, this, &ofApp::nbackCharHiddenCallback);
}

//--------------------------------------------------------------
void ofApp::seekFrameCallback(bool &val){
#ifdef VERBOSE_MODE
    cout << __PRETTY_FUNCTION__ << endl;
#endif
    if(seek.isInitialized()){
        if(_is_recording){
            cv::Mat buf;
            seek.getRawCVFrame(buf);
            ofxSeekThermal::writepgm(buf, _recording_path + "/" + _file_prefix.get(),
                                     _seek_frame_number, "", false);
            std::ostringstream pgmfilename;
            pgmfilename << _file_prefix.get();
            pgmfilename << std::internal << std::setfill('0') << std::setw(6);
            pgmfilename << _seek_frame_number;
            pgmfilename << ".pgm";
            csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                    "-",
                                    pgmfilename.str(),
                                    "-", nback.getLastCharacter(), "-", "-");
            _seek_frame_number++;
        }
    }
}

//--------------------------------------------------------------
void ofApp::nbackNewCharCallback(std::string &val){
#ifdef VERBOSE_MODE
    cout << __PRETTY_FUNCTION__ << endl;
#endif
    if(_is_recording){
        csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                "-", "-", "NEW", val, "-", "-");
    }
}

//--------------------------------------------------------------
void ofApp::nbackCharHiddenCallback(bool &val){
#ifdef VERBOSE_MODE
    cout << __PRETTY_FUNCTION__ << endl;
#endif
    if(_is_recording){
        csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                "-", "-", "HIDDEN", nback.getLastCharacter(), "-", "-");
    }
}

//--------------------------------------------------------------
std::string ofApp::nbackResultToString(int result){
    switch(result){
        case 0:
            return "FALSE";
        case 1:
            return "TRUE";
        default:
            return "NONE";
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

void ofApp::exitGui(ofEventArgs & args){
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
        if(_is_recording){
            csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                    "-", "-", "START", nback.getLastCharacter(), "-", "-");
        }
    }else{
        nback.stop();
        if(_is_recording){
            csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                    "-", "-", "STOP", nback.getLastCharacter(), "-", "-");
        }
    }
}

void ofApp::recordingCallback(bool & val){
#ifdef VERBOSE_MODE
    cout << __PRETTY_FUNCTION__ << val << endl;
#endif
    if(val){    // START
        if(!ofFilePath::isAbsolute(_file_path.get())){
            _recording_path = ofFilePath::getAbsolutePath(_file_path.get());
        }else{
            _recording_path = _file_path.get();
        }
        if(!ofDirectory::doesDirectoryExist(_recording_path)){
            ofDirectory::createDirectory(_recording_path);
        }else{
            ofLogNotice("Directory is already there so another one will be made");
            _recording_path += "-" + std::to_string(ofGetUnixTime());
            ofDirectory::createDirectory(_recording_path);
        }
        csv.start(_csv_header, _recording_path+"/log.csv");
        csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                "START", "-", "-", nback.getLastCharacter(), "-", "-");
        _seek_frame_number = 0;
        _is_recording = true;
    }else{      // STOP
        csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                "STOP", "-", "-", "-", "-", "-");
        csv.stop();
        _is_recording = false;
    }
}

void ofApp::killAppCallback(bool & val){
#ifdef VERBOSE_MODE
    cout << __PRETTY_FUNCTION__ << val << endl;
#endif
    if(val){
        ofExit();
    }
}

