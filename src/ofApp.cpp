#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	// set frame rate at 60fps constant
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    
	// seek camera setup
    seek.setup(OFX_SEEK_THERMAL_CAM_COMPACT);
	// OpenCV false color list:
	// https://docs.opencv.org/3.4/d3/d50/group__imgproc__colormap.html
    seek.setCVColorMap(cv::COLORMAP_BONE);
    seek.setVerbose(false);
    _img.allocate(THERMAL_WIDTH, THERMAL_HEIGHT, OF_IMAGE_COLOR);
    ofAddListener(seek.new_frame_evt, this, &ofApp::seekNewFrameCallback);
    
    // webcam setup
    for(auto &d : video.listDevices()){
        cout << d.id << " / " << d.deviceName << endl;
    }
    video.setDeviceID(1);
    video.setDesiredFrameRate(30);
    video.initGrabber(THERMAL_WIDTH, THERMAL_HEIGHT);
    
	// nback test setup
    nback.setup(NBACK_COUNT_PRAC,
				NBACK_CHAR_TIME_MS, NBACK_BLANK_TIME_MS, NBACK_RANDOMNESS_RATE,
				RYO_FONT_PATH);
    ofAddListener(nback.new_char_evt, this, &ofApp::nbackNewCharCallback);
    ofAddListener(nback.char_hidden_evt, this, &ofApp::nbackCharHiddenCallback);
	
	// study display config
	font.load(RYO_FONT_PATH, 64);
	
	// audio setup
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
	// look up sound files
    this->lookupFilesInDir(PRACTICE_PATH, "mp3", _practice_sounds, true);
	this->lookupFilesInDir(HCLHV_PATH, "mp3", _hclhv_sounds, true);
	this->lookupFilesInDir(HCLLV_PATH, "mp3", _hcllv_sounds, true);
	this->lookupFilesInDir(LCLHV_PATH, "mp3", _lclhv_sounds, true);
	this->lookupFilesInDir(LCLLV_PATH, "mp3", _lcllv_sounds, true);
    player.setLoop(false);
 
    // variables init
	_recording_path = "";
	_is_recording = false;
	_study_scene_main_phrase = "";
	_study_scene_sub_phrase = "";
	_study_phrase_sub_to_main_ratio = 0.4f;
	_study_scenario_count = 0;
    _study_scenario_counts.clear();
    _current_audio_pos = 0;
    _study_state = "-";
    _prev_progress_time = 0.0f;
    _next_progress_time = 0.0f;
    _current_sound_which = "-";
    _b_music_already_playing = false;
}

//--------------------------------------------------------------
void ofApp::update(){
    if(seek.isInitialized()){
		// if SeekThermal camera is tethered and a new frame is captured
		// this will be updated at each frame update of ofApp
		// and frame data recording will be processed in
		// ofApp::seekNewFrameCallback
        if(seek.isFrameNew()){
            _img.setFromPixels(seek.getVisualizePixels());
        }
    }
    if(video.isInitialized()){
        video.update();
    }
    if(nback.isTestRunning()){
        nback.update();
    }
    if(_whole_study_running){
		switch(_study_scenario_count){
			case 0:     // NONE
				break;
			case 1:     // climate init (intro)
				break;
			case 2:     // 1 minute calib
				break;
			case 3:     // practice intro
				break;
			case 4:     // practice session
                if(!_b_music_already_playing){
                    if(player.isLoaded()){
                        player.unload();
                        if(_current_audio_pos >= _practice_sounds.size()-1){
                            _current_audio_pos = 0;
                            this->studyScenarioChangeCallback();
                            break;
                        }else{
                            _current_audio_pos++;
                        }
                    }
                    player.load(_practice_sounds[_current_audio_pos]);
                    _current_sound_which = _practice_sounds[_current_audio_pos];
                    player.play();
                    _b_music_already_playing = true;
                }else{
                    if(!player.isPlaying()){
                        _b_music_already_playing = false;
                    }
                }
                break;
			case 7:     // LCLLV
                if(!_b_music_already_playing){
                    if(player.isLoaded()){
                        player.unload();
                        if(_current_audio_pos >= _lcllv_sounds.size()-1){
                            _current_audio_pos = 0;
                            this->studyScenarioChangeCallback();
                            break;
                        }else{
                            _current_audio_pos++;
                        }
                    }
                    player.load(_lcllv_sounds[_current_audio_pos]);
                    _current_sound_which = _lcllv_sounds[_current_audio_pos];
                    player.play();
                    _b_music_already_playing = true;
                }else{
                    if(!player.isPlaying()){
                        _b_music_already_playing = false;
                    }
                }
                break;
			case 9:     // LCLHV
                if(!_b_music_already_playing){
                    if(player.isLoaded()){
                        player.unload();
                        if(_current_audio_pos >= _lclhv_sounds.size()-1){
                            _current_audio_pos = 0;
                            this->studyScenarioChangeCallback();
                            break;
                        }else{
                            _current_audio_pos++;
                        }
                    }
                    player.load(_lclhv_sounds[_current_audio_pos]);
                    _current_sound_which = _lclhv_sounds[_current_audio_pos];
                    player.play();
                    _b_music_already_playing = true;
                }else{
                    if(!player.isPlaying()){
                        _b_music_already_playing = false;
                    }
                }
                break;
			case 11:    // HCLLV
                if(!_b_music_already_playing){
                    if(player.isLoaded()){
                        player.unload();
                        if(_current_audio_pos >= _hcllv_sounds.size()-1){
                            _current_audio_pos = 0;
                            this->studyScenarioChangeCallback();
                            break;
                        }else{
                            _current_audio_pos++;
                        }
                    }
                    player.load(_hcllv_sounds[_current_audio_pos]);
                    _current_sound_which = _hcllv_sounds[_current_audio_pos];
                    player.play();
                    _b_music_already_playing = true;
                }else{
                    if(!player.isPlaying()){
                        _b_music_already_playing = false;
                    }
                }
                break;
			case 13:    // HCLHV
                if(!_b_music_already_playing){
                    if(player.isLoaded()){
                        player.unload();
                        if(_current_audio_pos >= _hclhv_sounds.size()-1){
                            _current_audio_pos = 0;
                            this->studyScenarioChangeCallback();
                            break;
                        }else{
                            _current_audio_pos++;
                        }
                    }
                    player.load(_hclhv_sounds[_current_audio_pos]);
                    _current_sound_which = _hclhv_sounds[_current_audio_pos];
                    player.play();
                    _b_music_already_playing = true;
                }else{
                    if(!player.isPlaying()){
                        _b_music_already_playing = false;
                    }
                }
                break;
			case 5:     // 5 minutes pause
			case 8:
			case 10:
			case 12:
				break;
			case 6:     // main intro
				break;
			case 14:    // end
				break;
		}
	}
}

//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer & buffer){
    // nothing to do
}

//--------------------------------------------------------------
void ofApp::draw(){
	// buffering these window info to variables so that
	// oF won't cause any framerate decreasing issue
	float w = ofGetWidth();
	float h = ofGetHeight();
	
	if(_nback_visible){
		nback.draw(w/2, h/2-h/5, w/2, w/2);
	}
	if(_whole_study_running){
        this->drawStringCenterWithRatio(_study_scene_main_phrase,
                                        w/2, h/2+h/4, 0.5);
        this->drawStringCenterWithRatio(_study_scene_sub_phrase,
                                        w/2, h/2+h/4+50, 0.3);
		switch(_study_scenario_count){
			case 0:     // NONE
				break;
			case 1:     // climate init (intro)
			case 2:     // 1 minute calib
			case 3:     // practice intro
			case 5:     // 5 minutes pause
			case 8:
			case 10:
			case 12:
			case 6:     // main intro
			case 14:    // end
                this->drawProgressBarCenterWithSize(w/2, h-30, w/2, 30,
                                                    _prev_progress_time, _next_progress_time);
				break;
            case 4:     // practice session
			case 7:     // LCLLV
			case 9:     // LCLHV
			case 11:    // HCLLV
			case 13:    // HCLHV
                break;
		}
	}
}

//--------------------------------------------------------------
void ofApp::exit(){
#ifdef VERBOSE_MODE
	cout << __PRETTY_FUNCTION__ << endl;
#endif
	// cleanup everything
	
	if(nback.isTestRunning() || _whole_study_running){
        csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                "ERROR",    // system_state (START/STOP/ERROR)
                                _study_state,    // study_state (START/STOP/CALIB/INTRO/PRACTICE/
                                        //              INTVL/HCLHV/HCLLV/LCLHV/LCLLV)
                                "-",    // frame_file_name  (*.pgm)
                                "-",    // nback_state  (START/STOP/NEW/HIDDEN)
                                "-",    // nback_character (current character)
                                "-",    // nback_response (subject's response)
                                "-",    // nback_true_or_false (T/F)
                                "-",    // music_state (START/STOP/ERROR)
                                _current_sound_which,    // music_id (*.mp3)
                                "-"     // music_pos (0.0-1.0)
                                );
		csv.stop();
		nback.stop();
	}
	seek.close();
    video.close();
	sound_stream.close();
	
	_img.clear();
	
	// event listeners needs to be cleared
	ofRemoveListener(seek.new_frame_evt, this, &ofApp::seekNewFrameCallback);
	ofRemoveListener(nback.new_char_evt, this, &ofApp::nbackNewCharCallback);
	ofRemoveListener(nback.char_hidden_evt, this, &ofApp::nbackCharHiddenCallback);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	// sync with GuiWindow::keyReleased
    switch(key){
        case OF_KEY_LEFT_SHIFT:     // TRUE
            if(!nback.isResponseSubmitted()){
                nback.submitResponse(true);
                csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                        "-",    // system_state (START/STOP/ERROR)
                                        _study_state,    // study_state (START/STOP/CALIB/INTRO/PRACTICE/
                                                //              INTVL/HCLHV/HCLLV/LCLHV/LCLLV)
                                        "-",    // frame_file_name  (*.pgm)
                                        "-",    // nback_state  (START/STOP/NEW/HIDDEN)
                                        nback.getLastCharacter(),    // nback_character (current character)
                                        "TRUE",    // nback_response (subject's response)
                                        this->nbackResultToString(nback.isLastResponseTrue()),    // nback_true_or_false (T/F)
                                        "-",    // music_state (START/STOP/ERROR)
                                        _current_sound_which,    // music_id (*.mp3)
                                        (player.isPlaying() ? std::to_string(player.getPosition())
                                                            : "-")     // music_pos (0.0-1.0)
                                        );
            }
            break;
        case OF_KEY_RIGHT_SHIFT:    // FALSE
            if(!nback.isResponseSubmitted()){
                nback.submitResponse(false);
                csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                        "-",    // system_state (START/STOP/ERROR)
                                        _study_state,    // study_state (START/STOP/INTRO/CALIB/INTVL/
                                                // HCLHV/HCLLV/LCLHV/LCLLV)
                                        "-",    // frame_file_name  (*.pgm)
                                        "-",    // nback_state  (START/STOP/NEW/HIDDEN)
                                        nback.getLastCharacter(),    // nback_character (current character)
                                        "FALSE",    // nback_response (subject's response)
                                        this->nbackResultToString(nback.isLastResponseTrue()),    // nback_true_or_false (T/F)
                                        "-",    // music_state (START/STOP/ERROR)
                                        _current_sound_which,    // music_id (*.mp3)
                                        (player.isPlaying() ? std::to_string(player.getPosition())
                                                            : "-")     // music_pos (0.0-1.0)
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
void ofApp::seekNewFrameCallback(bool &val){
#ifdef VERBOSE_MODE
    cout << __PRETTY_FUNCTION__ << endl;
#endif
    if(seek.isInitialized()){
        if(_is_recording){
			// if data recording is enabled, each frame will be
			// stored to the recording dir in .pgm format
			// (16bit depth monochrome image)
            cv::Mat buf;
            seek.getRawCVFrame(buf);
            ofxSeekThermal::writepgm(buf, _recording_path + "/" + _file_prefix.get(),
                                     _seek_frame_number, "", false);
			// following code is same one from the ofxSeekThermal::writepgm
			// but do that again here to reproduce the file name
            std::ostringstream pgmfilename;
            pgmfilename << _file_prefix.get();
            pgmfilename << std::internal << std::setfill('0') << std::setw(6);
            pgmfilename << _seek_frame_number;
            pgmfilename << ".pgm";
            csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                    "-",    // system_state (START/STOP/ERROR)
                                    _study_state,    // study_state (START/STOP/INTRO/CALIB/INTVL/
                                            // HCLHV/HCLLV/LCLHV/LCLLV)
                                    pgmfilename.str(),    // frame_file_name  (*.pgm)
                                    "-",    // nback_state  (START/STOP/NEW/HIDDEN)
                                    nback.getLastCharacter(),    // nback_character (current character)
                                    "-",    // nback_response (subject's response)
                                    "-",    // nback_true_or_false (T/F)
                                    "-",    // music_state (START/STOP/ERROR)
                                    _current_sound_which,    // music_id (*.mp3)
                                    (player.isPlaying() ? std::to_string(player.getPosition())
                                                            : "-")     // music_pos (0.0-1.0)
                                    );
                                    
            std::ostringstream imgfilename;
            imgfilename << _file_prefix.get();
            imgfilename << std::internal << std::setfill('0') << std::setw(6);
            imgfilename << _seek_frame_number;
            imgfilename << ".jpg";
            ofSaveImage(video.getPixels(),
                        _recording_path + "/" + imgfilename.str());
            _seek_frame_number++;
        }
    }
}

//--------------------------------------------------------------
void ofApp::nbackNewCharCallback(std::string &val){
#ifdef VERBOSE_MODE
    cout << __PRETTY_FUNCTION__ << endl;
#endif
	// when new character is updated to Nback task
    if(_is_recording){
        csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                "-",    // system_state (START/STOP/ERROR)
                                _study_state,    // study_state (START/STOP/INTRO/CALIB/INTVL/
                                        // HCLHV/HCLLV/LCLHV/LCLLV)
                                "-",    // frame_file_name  (*.pgm)
                                "NEW",    // nback_state  (START/STOP/NEW/HIDDEN)
                                val,    // nback_character (current character)
                                "-",    // nback_response (subject's response)
                                "-",    // nback_true_or_false (T/F)
                                "-",    // music_state (START/STOP/ERROR)
                                _current_sound_which,    // music_id (*.mp3)
                                (player.isPlaying() ? std::to_string(player.getPosition())
                                                            : "-")     // music_pos (0.0-1.0)
                                );
    }
}

//--------------------------------------------------------------
void ofApp::nbackCharHiddenCallback(bool &val){
#ifdef VERBOSE_MODE
    cout << __PRETTY_FUNCTION__ << endl;
#endif
	// when the current character in Nback task gets hidden
    if(_is_recording){
        csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                "-",    // system_state (START/STOP/ERROR)
                                _study_state,    // study_state (START/STOP/INTRO/CALIB/INTVL/
                                        // HCLHV/HCLLV/LCLHV/LCLLV)
                                "-",    // frame_file_name  (*.pgm)
                                "HIDDEN",    // nback_state  (START/STOP/NEW/HIDDEN)
                                nback.getLastCharacter(),    // nback_character (current character)
                                "-",    // nback_response (subject's response)
                                "-",    // nback_true_or_false (T/F)
                                "-",    // music_state (START/STOP/ERROR)
                                _current_sound_which,    // music_id (*.mp3)
                                (player.isPlaying() ? std::to_string(player.getPosition())
                                                            : "-")     // music_pos (0.0-1.0)
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
void ofApp::nbackRunCallback(bool & val){
#ifdef VERBOSE_MODE
	cout << __PRETTY_FUNCTION__ << val << endl;
#endif
	// when Nback task started/stopped
	if(val){
		nback.start();
		if(_is_recording){
			csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
									"-",    // system_state (START/STOP/ERROR)
									_study_state,    // study_state (START/STOP/INTRO/CALIB/INTVL/
											// HCLHV/HCLLV/LCLHV/LCLLV)
									"-",    // frame_file_name  (*.pgm)
									"START",    // nback_state  (START/STOP/NEW/HIDDEN)
									nback.getLastCharacter(),    // nback_character (current character)
									"-",    // nback_response (subject's response)
									"-",    // nback_true_or_false (T/F)
									"-",    // music_state (START/STOP/ERROR)
									_current_sound_which,    // music_id (*.mp3)
									(player.isPlaying() ? std::to_string(player.getPosition())
                                                            : "-")     // music_pos (0.0-1.0)
									);
		}
	}else{
		nback.stop();
		if(_is_recording){
			csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
									"-",    // system_state (START/STOP/ERROR)
									_study_state,    // study_state (START/STOP/INTRO/CALIB/INTVL/
											// HCLHV/HCLLV/LCLHV/LCLLV)
									"-",    // frame_file_name  (*.pgm)
									"STOP",    // nback_state  (START/STOP/NEW/HIDDEN)
									nback.getLastCharacter(),    // nback_character (current character)
									"-",    // nback_response (subject's response)
									"-",    // nback_true_or_false (T/F)
									"-",    // music_state (START/STOP/ERROR)
									_current_sound_which,    // music_id (*.mp3)
									(player.isPlaying() ? std::to_string(player.getPosition())
                                                            : "-")     // music_pos (0.0-1.0)
									);
		}
	}
}

//--------------------------------------------------------------
void ofApp::drawStringCenterWithRatio(std::string message,
									  float center_x, float center_y, float rate){
    // just a simple utility function to draw a message on a window
	// with a specific scaling amount
	ofRectangle r = font.getStringBoundingBox(message, 0, 0);
	
	ofPushMatrix();
		ofTranslate(center_x-(r.width/2.0f*rate), center_y-(r.height/2.0f*rate));
		if(rate<1.0f || rate>1.0f){
			ofScale(rate);
		}
		font.drawString(message, 0.0, 0.0);
	ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::drawProgressBarCenterWithSize(float center_x, float center_y,
                                          float size_w, float size_h,
                                          float prev_time, float next_time){
    // just a simple utility function to draw a progress bar
    // in somewhere by specifying the center position and the size
    // of it
    float current_time = ofGetElapsedTimef();
    float rate = ofMap(current_time, prev_time, next_time, 0.0, 1.0);
    
    ofPushMatrix();
        ofTranslate(center_x-size_w/2, center_y-size_h/2);
        ofPushStyle();
            ofNoFill();
            ofDrawRectangle(0, 0, size_w, size_h);
            ofFill();
            ofDrawRectangle(0, 0, size_w*rate, size_h);
        ofPopStyle();
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::studyScenarioChangeCallback(){
#ifdef VERBOSE_MODE
    cout << __PRETTY_FUNCTION__ << endl;
#endif
	// study scenario control
	// observe the value of _study_scenario_count to control what to
	// display and what to change in the system
    if(_whole_study_running){
        if(_study_scenario_counts.size()){
            _study_scenario_counts.erase(_study_scenario_counts.begin());
            _study_scenario_count = _study_scenario_counts[0];
        }else{
            _study_scenario_count = 0;
            _recording = false;
        }
    }
    _prev_progress_time = 0.0f;
    _next_progress_time = 0.0f;
    _current_sound_which = "-";
    _current_audio_pos = 0;
    _b_music_already_playing = false;
    
    switch(_study_scenario_count){
        case 0:     // NONE
            _study_scene_main_phrase = "";
            _study_scene_sub_phrase = "";
            break;
        case 1:     // climate init (intro)
            _study_state = "INTRO";
            if(_is_recording){
                csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                        "-",    // system_state (START/STOP/ERROR)
                                        _study_state,    // study_state (START/STOP/INTRO/CALIB/INTVL/
                                                // HCLHV/HCLLV/LCLHV/LCLLV)
                                        "-",    // frame_file_name  (*.pgm)
                                        "-",    // nback_state  (START/STOP/NEW/HIDDEN)
                                        nback.getLastCharacter(),    // nback_character (current character)
                                        "-",    // nback_response (subject's response)
                                        "-",    // nback_true_or_false (T/F)
                                        "-",    // music_state (START/STOP/ERROR)
                                        _current_sound_which,    // music_id (*.mp3)
                                        "-"     // music_pos (0.0-1.0)
                                        );
            }
            _study_scene_main_phrase = STUDY_INTRO_MESSAGE_MAIN;
            _study_scene_sub_phrase = STUDY_INTRO_MESSAGE_SUB;
            
            if(_whole_study_running){
                _prev_progress_time = ofGetElapsedTimef();
                _next_progress_time = _prev_progress_time + STUDY_INTRO_TIME / 1000; // 10mins
                _thread = std::thread([this](){
                    std::this_thread::sleep_for(std::chrono::milliseconds(STUDY_INTRO_TIME));
                    this->studyScenarioChangeCallback();
                });
                threadMap[_thread_name] = _thread.native_handle();
                _thread.detach();
            }
            break;
        case 2:     // 1 minute calib
            _study_state = "CALIB";
            if(_is_recording){
                csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                        "-",    // system_state (START/STOP/ERROR)
                                        _study_state,    // study_state (START/STOP/INTRO/CALIB/INTVL/
                                                // HCLHV/HCLLV/LCLHV/LCLLV)
                                        "-",    // frame_file_name  (*.pgm)
                                        "-",    // nback_state  (START/STOP/NEW/HIDDEN)
                                        nback.getLastCharacter(),    // nback_character (current character)
                                        "-",    // nback_response (subject's response)
                                        "-",    // nback_true_or_false (T/F)
                                        "-",    // music_state (START/STOP/ERROR)
                                        _current_sound_which,    // music_id (*.mp3)
                                        "-"     // music_pos (0.0-1.0)
                                        );
            }
            _study_scene_main_phrase = STUDY_CALIB_MESSAGE_MAIN;
            _study_scene_sub_phrase = STUDY_CALIB_MESSAGE_SUB;
            
            if(_whole_study_running){
                _thread = std::thread([this](){
                    _prev_progress_time = ofGetElapsedTimef();
                    _next_progress_time = _prev_progress_time + STUDY_CALIB_TIME / 1000; // 60 @ sec = 1min
                    std::this_thread::sleep_for(std::chrono::milliseconds(STUDY_CALIB_TIME));
                    this->studyScenarioChangeCallback();
                });
                threadMap[_thread_name] = _thread.native_handle();
                _thread.detach();
            }
            break;
        case 3:     // practice intro
            _study_state = "INTRO";
            if(_is_recording){
                csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                        "-",    // system_state (START/STOP/ERROR)
                                        _study_state,    // study_state (START/STOP/INTRO/CALIB/INTVL/
                                                // HCLHV/HCLLV/LCLHV/LCLLV)
                                        "-",    // frame_file_name  (*.pgm)
                                        "-",    // nback_state  (START/STOP/NEW/HIDDEN)
                                        nback.getLastCharacter(),    // nback_character (current character)
                                        "-",    // nback_response (subject's response)
                                        "-",    // nback_true_or_false (T/F)
                                        "-",    // music_state (START/STOP/ERROR)
                                        _current_sound_which,    // music_id (*.mp3)
                                        (player.isPlaying() ? std::to_string(player.getPosition())
                                                            : "-")     // music_pos (0.0-1.0)
                                        );
            }
            _study_scene_main_phrase = STUDY_PRACTICE_MESSAGE_MAIN;
            _study_scene_sub_phrase = STUDY_PRACTICE_MESSAGE_SUB;
            nback.resizeSize(NBACK_COUNT_PRAC);
            _nback_visible = true;
            _nback_running = true;
            
            if(_whole_study_running){
                _prev_progress_time = ofGetElapsedTimef();
                _next_progress_time = _prev_progress_time + STUDY_CALIB_TIME / 1000; // 60 @ sec = 1min
                _thread = std::thread([this](){
                    std::this_thread::sleep_for(std::chrono::milliseconds(STUDY_CALIB_TIME));
                    this->studyScenarioChangeCallback();
                });
                threadMap[_thread_name] = _thread.native_handle();
                _thread.detach();
            }
            break;
        case 4:     // practice session
            _study_state = "PRACTICE";
            if(_is_recording){
                csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                        "-",    // system_state (START/STOP/ERROR)
                                        _study_state,    // study_state (START/STOP/INTRO/CALIB/INTVL/
                                                // HCLHV/HCLLV/LCLHV/LCLLV)
                                        "-",    // frame_file_name  (*.pgm)
                                        "-",    // nback_state  (START/STOP/NEW/HIDDEN)
                                        nback.getLastCharacter(),    // nback_character (current character)
                                        "-",    // nback_response (subject's response)
                                        "-",    // nback_true_or_false (T/F)
                                        "-",    // music_state (START/STOP/ERROR)
                                        _current_sound_which,    // music_id (*.mp3)
                                        (player.isPlaying() ? std::to_string(player.getPosition())
                                                            : "-")     // music_pos (0.0-1.0)
                                        );
            }
            _study_scene_main_phrase = STUDY_RUNNING_MESSAGE_MAIN;
            _study_scene_sub_phrase = STUDY_RUNNING_MESSAGE_SUB;
            _b_music_already_playing = false;
            break;
        case 7:     // LCLLV
            _study_state = "LCLLV";
            if(_is_recording){
                csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                        "-",    // system_state (START/STOP/ERROR)
                                        _study_state,    // study_state (START/STOP/INTRO/CALIB/INTVL/
                                                // HCLHV/HCLLV/LCLHV/LCLLV)
                                        "-",    // frame_file_name  (*.pgm)
                                        "-",    // nback_state  (START/STOP/NEW/HIDDEN)
                                        nback.getLastCharacter(),    // nback_character (current character)
                                        "-",    // nback_response (subject's response)
                                        "-",    // nback_true_or_false (T/F)
                                        "-",    // music_state (START/STOP/ERROR)
                                        _current_sound_which,    // music_id (*.mp3)
                                        (player.isPlaying() ? std::to_string(player.getPosition())
                                                            : "-")     // music_pos (0.0-1.0)
                                        );
            }
            _study_scene_main_phrase = STUDY_RUNNING_MESSAGE_MAIN;
            _study_scene_sub_phrase = STUDY_RUNNING_MESSAGE_SUB;
            nback.resizeSize(NBACK_COUNT_EASY);
            _nback_visible = true;
            _nback_running = true;
            _b_music_already_playing = false;
            break;
        case 9:     // LCLHV
            _study_state = "LCLHV";
            if(_is_recording){
                csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                        "-",    // system_state (START/STOP/ERROR)
                                        _study_state,    // study_state (START/STOP/INTRO/CALIB/INTVL/
                                                // HCLHV/HCLLV/LCLHV/LCLLV)
                                        "-",    // frame_file_name  (*.pgm)
                                        "-",    // nback_state  (START/STOP/NEW/HIDDEN)
                                        nback.getLastCharacter(),    // nback_character (current character)
                                        "-",    // nback_response (subject's response)
                                        "-",    // nback_true_or_false (T/F)
                                        "-",    // music_state (START/STOP/ERROR)
                                        _current_sound_which,    // music_id (*.mp3)
                                        (player.isPlaying() ? std::to_string(player.getPosition())
                                                            : "-")     // music_pos (0.0-1.0)
                                        );
            }
            _study_scene_main_phrase = STUDY_RUNNING_MESSAGE_MAIN;
            _study_scene_sub_phrase = STUDY_RUNNING_MESSAGE_SUB;
            nback.resizeSize(NBACK_COUNT_EASY);
            _nback_visible = true;
            _nback_running = true;
            _b_music_already_playing = false;
            break;
        case 11:    // HCLLV
            _study_state = "HCLLV";
            if(_is_recording){
                csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                        "-",    // system_state (START/STOP/ERROR)
                                        _study_state,    // study_state (START/STOP/INTRO/CALIB/INTVL/
                                                // HCLHV/HCLLV/LCLHV/LCLLV)
                                        "-",    // frame_file_name  (*.pgm)
                                        "-",    // nback_state  (START/STOP/NEW/HIDDEN)
                                        nback.getLastCharacter(),    // nback_character (current character)
                                        "-",    // nback_response (subject's response)
                                        "-",    // nback_true_or_false (T/F)
                                        "-",    // music_state (START/STOP/ERROR)
                                        _current_sound_which,    // music_id (*.mp3)
                                        (player.isPlaying() ? std::to_string(player.getPosition())
                                                            : "-")     // music_pos (0.0-1.0)
                                        );
            }
            _study_scene_main_phrase = STUDY_RUNNING_MESSAGE_MAIN;
            _study_scene_sub_phrase = STUDY_RUNNING_MESSAGE_SUB;
            nback.resizeSize(NBACK_COUNT_DIFF);
            _nback_visible = true;
            _nback_running = true;
            _b_music_already_playing = false;
            break;
        case 13:    // HCLHV
            _study_state = "HCLHV";
            if(_is_recording){
                csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                        "-",    // system_state (START/STOP/ERROR)
                                        _study_state,    // study_state (START/STOP/INTRO/CALIB/INTVL/
                                                // HCLHV/HCLLV/LCLHV/LCLLV)
                                        "-",    // frame_file_name  (*.pgm)
                                        "-",    // nback_state  (START/STOP/NEW/HIDDEN)
                                        nback.getLastCharacter(),    // nback_character (current character)
                                        "-",    // nback_response (subject's response)
                                        "-",    // nback_true_or_false (T/F)
                                        "-",    // music_state (START/STOP/ERROR)
                                        _current_sound_which,    // music_id (*.mp3)
                                        (player.isPlaying() ? std::to_string(player.getPosition())
                                                            : "-")     // music_pos (0.0-1.0)
                                        );
            }
            _study_scene_main_phrase = STUDY_RUNNING_MESSAGE_MAIN;
            _study_scene_sub_phrase = STUDY_RUNNING_MESSAGE_SUB;
            nback.resizeSize(NBACK_COUNT_DIFF);
            _nback_visible = true;
            _nback_running = true;
            _b_music_already_playing = false;
            break;
        case 5:     // 5 minutes pause
        case 8:
        case 10:
        case 12:
            _study_state = "INTVL";
            if(_is_recording){
                csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                        "-",    // system_state (START/STOP/ERROR)
                                        _study_state,    // study_state (START/STOP/INTRO/CALIB/INTVL/
                                                // HCLHV/HCLLV/LCLHV/LCLLV)
                                        "-",    // frame_file_name  (*.pgm)
                                        "-",    // nback_state  (START/STOP/NEW/HIDDEN)
                                        nback.getLastCharacter(),    // nback_character (current character)
                                        "-",    // nback_response (subject's response)
                                        "-",    // nback_true_or_false (T/F)
                                        "-",    // music_state (START/STOP/ERROR)
                                        _current_sound_which,    // music_id (*.mp3)
                                        (player.isPlaying() ? std::to_string(player.getPosition())
                                                            : "-")     // music_pos (0.0-1.0)
                                        );
            }
            _study_scene_main_phrase = STUDY_INTERVAL_MESSAGE_MAIN;
            _study_scene_sub_phrase = STUDY_INTERVAL_MESSAGE_SUB;
            _nback_visible = false;
            _nback_running = false;
            
            if(_whole_study_running){
                _prev_progress_time = ofGetElapsedTimef();
                _next_progress_time = _prev_progress_time + STUDY_INTVL_TIME / 1000; // 5mins
                _thread = std::thread([this](){
                    std::this_thread::sleep_for(std::chrono::milliseconds(STUDY_INTVL_TIME));
                    this->studyScenarioChangeCallback();
                });
                threadMap[_thread_name] = _thread.native_handle();
                _thread.detach();
            }
            break;
        case 6:     // main intro
            _study_state = "INTRO";
            if(_is_recording){
                csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                        "-",    // system_state (START/STOP/ERROR)
                                        _study_state,    // study_state (START/STOP/INTRO/CALIB/INTVL/
                                                // HCLHV/HCLLV/LCLHV/LCLLV)
                                        "-",    // frame_file_name  (*.pgm)
                                        "-",    // nback_state  (START/STOP/NEW/HIDDEN)
                                        nback.getLastCharacter(),    // nback_character (current character)
                                        "-",    // nback_response (subject's response)
                                        "-",    // nback_true_or_false (T/F)
                                        "-",    // music_state (START/STOP/ERROR)
                                        _current_sound_which,    // music_id (*.mp3)
                                        (player.isPlaying() ? std::to_string(player.getPosition())
                                                            : "-")     // music_pos (0.0-1.0)
                                        );
            }
            _study_scene_main_phrase = STUDY_MAIN_MESSAGE_MAIN;
            _study_scene_sub_phrase = STUDY_MAIN_MESSAGE_SUB;
            nback.resizeSize(NBACK_COUNT_PRAC);
            _nback_visible = true;
            _nback_running = true;
            
            if(_whole_study_running){
                _prev_progress_time = ofGetElapsedTimef();
                _next_progress_time = _prev_progress_time + STUDY_CALIB_TIME / 1000; // 60 @ sec = 1min
                _thread = std::thread([this](){
                    std::this_thread::sleep_for(std::chrono::milliseconds(STUDY_CALIB_TIME));
                    this->studyScenarioChangeCallback();
                });
                threadMap[_thread_name] = _thread.native_handle();
                _thread.detach();
            }
            break;
        case 14:    // end
            _study_state = "STOP";
            if(_is_recording){
                csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                        "-",    // system_state (START/STOP/ERROR)
                                        _study_state,    // study_state (START/STOP/INTRO/CALIB/INTVL/
                                                // HCLHV/HCLLV/LCLHV/LCLLV)
                                        "-",    // frame_file_name  (*.pgm)
                                        "-",    // nback_state  (START/STOP/NEW/HIDDEN)
                                        nback.getLastCharacter(),    // nback_character (current character)
                                        "-",    // nback_response (subject's response)
                                        "-",    // nback_true_or_false (T/F)
                                        "-",    // music_state (START/STOP/ERROR)
                                        _current_sound_which,    // music_id (*.mp3)
                                        (player.isPlaying() ? std::to_string(player.getPosition())
                                                            : "-")     // music_pos (0.0-1.0)
                                        );
            }
            _study_scene_main_phrase = STUDY_END_MESSAGE_MAIN;
            _study_scene_sub_phrase = STUDY_END_MESSAGE_SUB;
            _nback_visible = false;
            _nback_running = false;
            
            if(_whole_study_running){
                _prev_progress_time = ofGetElapsedTimef();
                _next_progress_time = _prev_progress_time + STUDY_CALIB_TIME / 1000; // 60 @ sec = 1min
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
	// just a utility function to stop the worker thread
	// in case if the study is suspended on the half way
    ofxRHUtilities::ThreadMap::const_iterator it = threadMap.find(tname);
    if(it != threadMap.end()){
        pthread_cancel(it->second);
        threadMap.erase(tname);
    }
}

//--------------------------------------------------------------
void ofApp::startWholeStudy(){
#ifdef VERBOSE_MODE
	cout << __PRETTY_FUNCTION__ << endl;
#endif
    _recording = true;
	csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                            "-",    // system_state (START/STOP/ERROR)
                            _study_state,    // study_state (START/STOP/INTRO/CALIB/INTVL/
                                    // HCLHV/HCLLV/LCLHV/LCLLV)
                            "-",    // frame_file_name  (*.pgm)
                            "-",    // nback_state  (START/STOP/NEW/HIDDEN)
                            nback.getLastCharacter(),    // nback_character (current character)
                            "-",    // nback_response (subject's response)
                            "-",    // nback_true_or_false (T/F)
                            "-",    // music_state (START/STOP/ERROR)
                            _current_sound_which,    // music_id (*.mp3)
                            (player.isPlaying() ? std::to_string(player.getPosition())
                                                : "-")     // music_pos (0.0-1.0)
                            );
    this->studyScenarioChangeCallback();
}

//--------------------------------------------------------------
void ofApp::stopWholeStudy(){
#ifdef VERBOSE_MODE
	cout << __PRETTY_FUNCTION__ << endl;
#endif
	_recording = false;
    _study_scenario_count = 0;
    this->studyScenarioChangeCallback();
}

//--------------------------------------------------------------
bool ofApp::lookupFilesInDir(std::string target_path,
							 std::string extension,
							 std::vector<std::string> &file_list,
							 bool clear_list_buffer){
#ifdef VERBOSE_MODE
	cout << __PRETTY_FUNCTION__ << endl;
#endif
	// lookup files with a specified extension in a certain directory
	// make sure target vector is cleared
	if(clear_list_buffer){
		if(file_list.size()){
			file_list.clear();
		}
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
void ofApp::setupGui(){
#ifdef VERBOSE_MODE
    cout << __PRETTY_FUNCTION__ << endl;
#endif
    proto_parameters.setName("test_parameters");
	proto_parameters.add(_nback_visible.set("Nback visible 4 debug", false));
    proto_parameters.add(_nback_running.set("Nback Run/Stop", false));
    proto_parameters.add(_recording.set("Recording", false));
    
    main_parameters.setName("main_parameters");
    main_parameters.add(_participant_id.set("Participant ID", "name_"));
    main_parameters.add(_file_path.set("Recording Path", "seek_frame_samples"));
    main_parameters.add(_file_prefix.set("File prefix", "frame_"));
    main_parameters.add(_whole_study_running.set("Whole study Run/Stop", false));
	main_parameters.add(_kill_app.set("Quit App?", false));
    
    _nback_running.addListener(this, &ofApp::nbackRunCallback);
    _recording.addListener(this, &ofApp::recordingCallback);
    _kill_app.addListener(this, &ofApp::killAppCallback);
    _whole_study_running.addListener(this, &ofApp::wholeStudyRunCallback);
    
    gui.setup();
#ifdef DEBUG_GUI_ENABLE
    gui.add(proto_parameters);
#endif
    gui.add(main_parameters);
}

//--------------------------------------------------------------
void ofApp::drawGui(ofEventArgs & args){
    _img.draw(0, 0, THERMAL_WIDTH * 1.5, THERMAL_HEIGHT * 1.5);
    video.draw(0, THERMAL_HEIGHT * 1.5, THERMAL_WIDTH * 1.5, THERMAL_HEIGHT * 1.5);
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::exitGui(ofEventArgs & args){
#ifdef VERBOSE_MODE
    cout << __PRETTY_FUNCTION__ << endl;
#endif
    _nback_running.removeListener(this, &ofApp::nbackRunCallback);
    _recording.removeListener(this, &ofApp::recordingCallback);
    _kill_app.removeListener(this, &ofApp::killAppCallback);
    _whole_study_running.removeListener(this, &ofApp::wholeStudyRunCallback);
}

//--------------------------------------------------------------
void ofApp::keyReleasedGui(ofKeyEventArgs & args){
#ifdef VERBOSE_MODE
	cout << __PRETTY_FUNCTION__ << endl;
#endif
	this->keyReleased(args.key);
}

//--------------------------------------------------------------
void ofApp::recordingCallback(bool & val){
#ifdef VERBOSE_MODE
    cout << __PRETTY_FUNCTION__ << val << endl;
#endif
    if(val){    // START
        _study_scenario_counts.clear();
        for(int i=0; i<15; i++){
            _study_scenario_counts.push_back(i);
        }
        // check xml file to store previous study order (order.xml, latin square)
        if(!studyOrderXml.load(STUDY_ORDER_KEEPER_XML)){
            // make new xml and save it
            studyOrderXml.clear();
            auto child = studyOrderXml.appendChild("order");
//		 * 7  - LCLLV
//		 * 9  - LCLHV
//		 * 11 - HCLLV
//		 * 13 - HCLHV
            child.appendChild("first").set(7);
            child.appendChild("second").set(9);
            child.appendChild("third").set(11);
            child.appendChild("fourth").set(13);
            studyOrderXml.save(STUDY_ORDER_KEEPER_XML);
        }else{
            auto order = studyOrderXml.findFirst("order");
            if(order){
                _study_scenario_counts[7] = order.getChild("first").getIntValue();
                _study_scenario_counts[9] = order.getChild("second").getIntValue();
                _study_scenario_counts[11] = order.getChild("third").getIntValue();
                _study_scenario_counts[13] = order.getChild("fourth").getIntValue();
                // update xml and save it
                studyOrderXml.clear();
                auto child = studyOrderXml.appendChild("order");
                child.appendChild("first").set(_study_scenario_counts[9]);
                child.appendChild("second").set(_study_scenario_counts[11]);
                child.appendChild("third").set(_study_scenario_counts[13]);
                child.appendChild("fourth").set(_study_scenario_counts[7]);
                studyOrderXml.save(STUDY_ORDER_KEEPER_XML);
            }
        }
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
        csv.start(_csv_header_only, _recording_path + "/" + _file_prefix.get() + "log.csv");
        _study_state = "START";
        csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                "START",    // system_state (START/STOP/ERROR)
                                _study_state,    // study_state (START/STOP/INTRO/CALIB/INTVL/
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
        _study_state = "STOP";
        csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                                _study_state,    // system_state (START/STOP/ERROR)
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
void ofApp::wholeStudyRunCallback(bool & val){
#ifdef VERBOSE_MODE
    cout << __PRETTY_FUNCTION__ << val << endl;
#endif
	this->cancelBackgroundThread(_thread_name);
	
	if(val){
		this->startWholeStudy();
	}else{
		this->stopWholeStudy();
	}
}

