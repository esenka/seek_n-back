#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	// set frame rate at 60fps constant
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    
	// seek camera setup
    seek.setup(OFX_SEEK_THERMAL_CAM_COMPACT);
    seek.setCVColorMap(cv::COLORMAP_BONE);
    seek.setVerbose(false);
    _img.allocate(THERMAL_WIDTH, THERMAL_HEIGHT, OF_IMAGE_COLOR);
    ofAddListener(seek.new_frame_evt, this, &ofApp::seekFrameCallback);
    
    // audio setup
    // SOUND
    sound_stream.printDeviceList();
    ofSoundStreamSettings sound_settings;
    sound_settings.setApi(ofSoundDevice::Api::OSX_CORE);
    sound_settings.setOutListener(this);
    sound_settings.sampleRate = SOUND_SAMPLING_RATE;
    sound_settings.numOutputChannels = 2;
    sound_settings.numInputChannels = 0;
    sound_settings.bufferSize = SOUND_BUFFER_SIZE;
    sound_stream.setup(sound_settings);
    //sound_stream.stop();
    
	// nback test setup
    nback.setup(3, 500, 2000, 0.2, ESEN_FONT_PATH);
    ofAddListener(nback.new_char_evt, this, &ofApp::nbackNewCharCallback);
    ofAddListener(nback.char_hidden_evt, this, &ofApp::nbackCharHiddenCallback);
    
	// csv export setup
    std::map<std::string, std::string>::iterator itr = _csv_header_and_value.begin();
    while(itr!=_csv_header_and_value.end()){
        _csv_header.push_back(itr->first);
        itr++;
    }
    
    _study_scenario_count = 0;
}

//--------------------------------------------------------------
void ofApp::update(){
    if(seek.isInitialized()){
        if(seek.isFrameNew()){
            _img.setFromPixels(seek.getVisualizePixels());
        }
    }
    if(nback.isTestRunning()){
        nback.update();
    }
}

//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer & buffer){
    // nothing to do
}

//--------------------------------------------------------------
void ofApp::draw(){
    nback.draw(512, 384, 400, 400);
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
                                        "-",    // system_state (START/STOP/ERROR)
                                        "-",    // study_state (START/STOP/INTRO/CALIB/INTVL/
                                                // HCLHV/HCLLV/LCLHV/LCLLV)
                                        "-",    // frame_file_name  (*.pgm)
                                        "-",    // nback_state  (START/STOP/NEW/HIDDEN)
                                        nback.getLastCharacter(),    // nback_character (current character)
                                        "TRUE",    // nback_response (subject's response)
                                        this->nbackResultToString(nback.isLastResponseTrue()),    // nback_true_or_false (T/F)
                                        "-",    // music_state (START/STOP/ERROR)
                                        "-",    // music_id (*.mp3)
                                        "-"     // music_pos (0.0-1.0)
                                        );
            }
            break;
        case OF_KEY_RIGHT_SHIFT:    // FALSE
            if(!nback.isResponseSubmitted()){
                nback.submitResponse(false);
                csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                        "-",    // system_state (START/STOP/ERROR)
                                        "-",    // study_state (START/STOP/INTRO/CALIB/INTVL/
                                                // HCLHV/HCLLV/LCLHV/LCLLV)
                                        "-",    // frame_file_name  (*.pgm)
                                        "-",    // nback_state  (START/STOP/NEW/HIDDEN)
                                        nback.getLastCharacter(),    // nback_character (current character)
                                        "FALSE",    // nback_response (subject's response)
                                        this->nbackResultToString(nback.isLastResponseTrue()),    // nback_true_or_false (T/F)
                                        "-",    // music_state (START/STOP/ERROR)
                                        "-",    // music_id (*.mp3)
                                        "-"     // music_pos (0.0-1.0)
                                        );
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
void ofApp::exit(){
#ifdef VERBOSE_MODE
    cout << __PRETTY_FUNCTION__ << endl;
#endif
    if(nback.isTestRunning()){
        csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                "ERROR",    // system_state (START/STOP/ERROR)
                                "-",    // study_state (START/STOP/INTRO/CALIB/INTVL/
                                        // HCLHV/HCLLV/LCLHV/LCLLV)
                                "-",    // frame_file_name  (*.pgm)
                                "-",    // nback_state  (START/STOP/NEW/HIDDEN)
                                "-",    // nback_character (current character)
                                "-",    // nback_response (subject's response)
                                "-",    // nback_true_or_false (T/F)
                                "-",    // music_state (START/STOP/ERROR)
                                "-",    // music_id (*.mp3)
                                "-"     // music_pos (0.0-1.0)
                                );
        csv.stop();
        nback.stop();
    }
    seek.close();
    sound_stream.close();
    
    _img.clear();
    
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
                                    "-",    // system_state (START/STOP/ERROR)
                                    "-",    // study_state (START/STOP/INTRO/CALIB/INTVL/
                                            // HCLHV/HCLLV/LCLHV/LCLLV)
                                    pgmfilename.str(),    // frame_file_name  (*.pgm)
                                    "-",    // nback_state  (START/STOP/NEW/HIDDEN)
                                    nback.getLastCharacter(),    // nback_character (current character)
                                    "-",    // nback_response (subject's response)
                                    "-",    // nback_true_or_false (T/F)
                                    "-",    // music_state (START/STOP/ERROR)
                                    "-",    // music_id (*.mp3)
                                    "-"     // music_pos (0.0-1.0)
                                    );
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
                                "-",    // system_state (START/STOP/ERROR)
                                "-",    // study_state (START/STOP/INTRO/CALIB/INTVL/
                                        // HCLHV/HCLLV/LCLHV/LCLLV)
                                "-",    // frame_file_name  (*.pgm)
                                "NEW",    // nback_state  (START/STOP/NEW/HIDDEN)
                                val,    // nback_character (current character)
                                "-",    // nback_response (subject's response)
                                "-",    // nback_true_or_false (T/F)
                                "-",    // music_state (START/STOP/ERROR)
                                "-",    // music_id (*.mp3)
                                "-"     // music_pos (0.0-1.0)
                                );
    }
}

//--------------------------------------------------------------
void ofApp::nbackCharHiddenCallback(bool &val){
#ifdef VERBOSE_MODE
    cout << __PRETTY_FUNCTION__ << endl;
#endif
    if(_is_recording){
        csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                "-",    // system_state (START/STOP/ERROR)
                                "-",    // study_state (START/STOP/INTRO/CALIB/INTVL/
                                        // HCLHV/HCLLV/LCLHV/LCLLV)
                                "-",    // frame_file_name  (*.pgm)
                                "HIDDEN",    // nback_state  (START/STOP/NEW/HIDDEN)
                                nback.getLastCharacter(),    // nback_character (current character)
                                "-",    // nback_response (subject's response)
                                "-",    // nback_true_or_false (T/F)
                                "-",    // music_state (START/STOP/ERROR)
                                "-",    // music_id (*.mp3)
                                "-"     // music_pos (0.0-1.0)
                                );
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

//--------------------------------------------------------------
bool ofApp::lookupFilesInDir(std::string target_path,
                             std::string extension,
                             std::vector<std::string> &file_list){
#ifdef VERBOSE_MODE
    cout << __PRETTY_FUNCTION__ << endl;
#endif

    // make sure target vector is cleared
    if(file_list.size()){
        file_list.clear();
    }
    
    ofDirectory dir(target_path);
    dir.allowExt(extension);
    dir.sort();
    if(dir.getFiles().size()){
        for(auto &f : dir.getFiles()){
            file_list.push_back(f.getAbsolutePath());
#ifdef VERBOSE_MODE
            cout << "FOUND: " << f.getAbsolutePath() << endl;
#endif
        }
        return true;
    }else{
        return false;
    }
}

//--------------------------------------------------------------
void ofApp::studyScenarioChangeCallback(){
#ifdef VERBOSE_MODE
    cout << __PRETTY_FUNCTION__ << endl;
#endif

    if(_whole_study_running){
        _study_scenario_count >= 12 ? _study_scenario_count = 0
                                    : _study_scenario_count++;
    }
    
    switch(_study_scenario_count){
        case 0:     // NONE
            _study_scene_main_phrase = "";
            _study_scene_sub_phrase = "";
            break;
        case 1:     // climate init (intro)
            _study_scene_main_phrase = STUDY_INTRO_MESSAGE_MAIN;
            _study_scene_sub_phrase = STUDY_INTRO_MESSAGE_SUB;
            if(_whole_study_running){
                _thread = std::thread([this](){
                    std::this_thread::sleep_for(std::chrono::milliseconds(STUDY_INTRO_TIME));
                    this->studyScenarioChangeCallback();
                });
                threadMap[_thread_name] = _thread.native_handle();
                _thread.detach();
            }
            break;
        case 2:     // 1 minute calib
            _study_scene_main_phrase = STUDY_CALIB_MESSAGE_MAIN;
            _study_scene_sub_phrase = STUDY_CALIB_MESSAGE_SUB;
            if(_whole_study_running){
                _thread = std::thread([this](){
                    std::this_thread::sleep_for(std::chrono::milliseconds(STUDY_CALIB_TIME));
                    this->studyScenarioChangeCallback();
                });
                threadMap[_thread_name] = _thread.native_handle();
                _thread.detach();
            }
            break;
        case 3:     // practice intro
            _study_scene_main_phrase = STUDY_PRACTICE_MESSAGE_MAIN;
            _study_scene_sub_phrase = STUDY_PRACTICE_MESSAGE_SUB;
            if(_whole_study_running){
                _thread = std::thread([this](){
                    std::this_thread::sleep_for(std::chrono::milliseconds(STUDY_CALIB_TIME));
                    this->studyScenarioChangeCallback();
                });
                threadMap[_thread_name] = _thread.native_handle();
                _thread.detach();
            }
            break;
        case 4:     // practice session
        case 7:     // LCLLV
        case 9:     // LCLHV
        case 11:    // HCLLV
        case 13:    // HCLHV
            _study_scene_main_phrase = STUDY_RUNNING_MESSAGE_MAIN;
            _study_scene_sub_phrase = STUDY_RUNNING_MESSAGE_SUB;
            break;
        case 5:     // 5 minutes pause
        case 8:
        case 10:
        case 12:
            _study_scene_main_phrase = STUDY_INTERVAL_MESSAGE_MAIN;
            _study_scene_sub_phrase = STUDY_INTERVAL_MESSAGE_SUB;
            if(_whole_study_running){
                _thread = std::thread([this](){
                    std::this_thread::sleep_for(std::chrono::milliseconds(STUDY_INTVL_TIME));
                    this->studyScenarioChangeCallback();
                });
                threadMap[_thread_name] = _thread.native_handle();
                _thread.detach();
            }
            break;
        case 6:     // main intro
            _study_scene_main_phrase = STUDY_MAIN_MESSAGE_MAIN;
            _study_scene_sub_phrase = STUDY_MAIN_MESSAGE_SUB;
            if(_whole_study_running){
                _thread = std::thread([this](){
                    std::this_thread::sleep_for(std::chrono::milliseconds(STUDY_CALIB_TIME));
                    this->studyScenarioChangeCallback();
                });
                threadMap[_thread_name] = _thread.native_handle();
                _thread.detach();
            }
            break;
        case 14:    // end
            _study_scene_main_phrase = STUDY_END_MESSAGE_MAIN;
            _study_scene_sub_phrase = STUDY_END_MESSAGE_SUB;
            if(_whole_study_running){
                _thread = std::thread([this](){
                    std::this_thread::sleep_for(std::chrono::milliseconds(STUDY_CALIB_TIME));
                    this->studyScenarioChangeCallback();
                });
                threadMap[_thread_name] = _thread.native_handle();
                _thread.detach();
            }
            break;
    }

}

//--------------------------------------------------------------
void ofApp::cancelBackgroundThread(const std::string &tname){
#ifdef VERBOSE_MODE
    cout << __PRETTY_FUNCTION__ << endl;
#endif
    ofxRHUtilities::ThreadMap::const_iterator it = threadMap.find(tname);
    if(it != threadMap.end()){
        pthread_cancel(it->second);
        threadMap.erase(tname);
    }
}

//--------------------------------------------------------------
void ofApp::setupGui(){
#ifdef VERBOSE_MODE
    cout << __PRETTY_FUNCTION__ << endl;
#endif
    proto_parameters.setName("test_parameters");
    proto_parameters.add(_nback_running.set("Nback Run/Stop", false));
    proto_parameters.add(_recording.set("Recording", false));
    proto_parameters.add(_kill_app.set("Quit App?", false));
    
    main_parameters.setName("main_parameters");
    main_parameters.add(_participant_id.set("Participant ID", "name_"));
    main_parameters.add(_file_path.set("Recording Path", "seek_frame_samples"));
    main_parameters.add(_file_prefix.set("File prefix", "frame_"));
    main_parameters.add(_whole_study_running.set("Whole study Run/Stop", false));
    
    _nback_running.addListener(this, &ofApp::nbackCallback);
    _recording.addListener(this, &ofApp::recordingCallback);
    _kill_app.addListener(this, &ofApp::killAppCallback);
    _whole_study_running.addListener(this, &ofApp::wholeStudyRunningCallback);
    
    gui.setup();
    gui.add(proto_parameters);
    gui.add(main_parameters);
}

//--------------------------------------------------------------
void ofApp::drawGui(ofEventArgs &args){
    _img.draw(0, 0, THERMAL_WIDTH * 2, THERMAL_HEIGHT * 2);
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::exitGui(ofEventArgs & args){
#ifdef VERBOSE_MODE
    cout << __PRETTY_FUNCTION__ << endl;
#endif
    _nback_running.removeListener(this, &ofApp::nbackCallback);
    _recording.removeListener(this, &ofApp::recordingCallback);
    _kill_app.removeListener(this, &ofApp::killAppCallback);
    _whole_study_running.removeListener(this, &ofApp::wholeStudyRunningCallback);
}

//--------------------------------------------------------------
void ofApp::nbackCallback(bool & val){
#ifdef VERBOSE_MODE
    cout << __PRETTY_FUNCTION__ << val << endl;
#endif
    if(val){
        nback.start();
        if(_is_recording){
            csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                    "-",    // system_state (START/STOP/ERROR)
                                    "-",    // study_state (START/STOP/INTRO/CALIB/INTVL/
                                            // HCLHV/HCLLV/LCLHV/LCLLV)
                                    "-",    // frame_file_name  (*.pgm)
                                    "START",    // nback_state  (START/STOP/NEW/HIDDEN)
                                    nback.getLastCharacter(),    // nback_character (current character)
                                    "-",    // nback_response (subject's response)
                                    "-",    // nback_true_or_false (T/F)
                                    "-",    // music_state (START/STOP/ERROR)
                                    "-",    // music_id (*.mp3)
                                    "-"     // music_pos (0.0-1.0)
                                    );
        }
    }else{
        nback.stop();
        if(_is_recording){
            csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                    "-",    // system_state (START/STOP/ERROR)
                                    "-",    // study_state (START/STOP/INTRO/CALIB/INTVL/
                                            // HCLHV/HCLLV/LCLHV/LCLLV)
                                    "-",    // frame_file_name  (*.pgm)
                                    "STOP",    // nback_state  (START/STOP/NEW/HIDDEN)
                                    nback.getLastCharacter(),    // nback_character (current character)
                                    "-",    // nback_response (subject's response)
                                    "-",    // nback_true_or_false (T/F)
                                    "-",    // music_state (START/STOP/ERROR)
                                    "-",    // music_id (*.mp3)
                                    "-"     // music_pos (0.0-1.0)
                                    );
        }
    }
}

//--------------------------------------------------------------
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
                                "START",    // system_state (START/STOP/ERROR)
                                "-",    // study_state (START/STOP/INTRO/CALIB/INTVL/
                                        // HCLHV/HCLLV/LCLHV/LCLLV)
                                "-",    // frame_file_name  (*.pgm)
                                "-",    // nback_state  (START/STOP/NEW/HIDDEN)
                                nback.getLastCharacter(),    // nback_character (current character)
                                "-",    // nback_response (subject's response)
                                "-",    // nback_true_or_false (T/F)
                                "-",    // music_state (START/STOP/ERROR)
                                "-",    // music_id (*.mp3)
                                "-"     // music_pos (0.0-1.0)
                                );
        _seek_frame_number = 0;
        _is_recording = true;
    }else{      // STOP
        csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                "STOP",    // system_state (START/STOP/ERROR)
                                "-",    // study_state (START/STOP/INTRO/CALIB/INTVL/
                                        // HCLHV/HCLLV/LCLHV/LCLLV)
                                "-",    // frame_file_name  (*.pgm)
                                "-",    // nback_state  (START/STOP/NEW/HIDDEN)
                                "-",    // nback_character (current character)
                                "-",    // nback_response (subject's response)
                                "-",    // nback_true_or_false (T/F)
                                "-",    // music_state (START/STOP/ERROR)
                                "-",    // music_id (*.mp3)
                                "-"     // music_pos (0.0-1.0)
                                );
        csv.stop();
        _is_recording = false;
    }
}

//--------------------------------------------------------------
void ofApp::killAppCallback(bool & val){
#ifdef VERBOSE_MODE
    cout << __PRETTY_FUNCTION__ << val << endl;
#endif
    if(val){
        ofExit();
    }
}

//--------------------------------------------------------------
void ofApp::wholeStudyRunningCallback(bool & val){
#ifdef VERBOSE_MODE
    cout << __PRETTY_FUNCTION__ << val << endl;
#endif
    if(val){
        this->studyScenarioChangeCallback();
    }else{
        this->cancelBackgroundThread(_thread_name);
        _study_scenario_count = 0;
        this->studyScenarioChangeCallback();
    }
}

