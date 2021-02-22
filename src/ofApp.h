#pragma once

#include "ofMain.h"
#include "ofxSeekThermal.h"
#include "opencv2/opencv.hpp"
#include "ofxNbackTest.hpp"
#include "RHUtils.h"
#include "ofxGui.h"

// font paths
#define ESEN_FONT_PATH "gt-pressura-regular.ttf"
#define RYO_FONT_PATH "/System/Library/Fonts/SFNS.ttf"

// paths to the mp3 files
// if it's in somewhere outside the project folder,
// you can specify the paths in absolute path format
// i.e. replacing "stimuli/HCLHV" to "/Users/esen/Documents/stimuli/HCLHV"
#define HCLHV_PATH "stimuli/HCLHV"
#define HCLLV_PATH "stimuli/HCLLV"
#define LCLHV_PATH "stimuli/LCLHV"
#define LCLLV_PATH "stimuli/LCLLV"

// TIMING_SETUP
#define STUDY_INTRO_TIME 1200000    // 20 minutes in MS
#define STUDY_CALIB_TIME 60000      // 1 minute in MS
#define STUDY_INTVL_TIME 300000     // 5 minutes in MS

#define TRACKS_FOR_EACH_SESSION 1

// SYSTEM SETUP
#define VERBOSE_MODE 1	// comment this line out to suppress verbose output

static std::map<std::string, std::string> _csv_header_and_value = {
    {"system_timestamp_millis", ""},
    {"system_state",            ""},
    {"frame_file_name",         ""},
    {"nback_state",             ""},
    {"nback_character",         ""},
    {"nback_response",          ""},
    {"nback_true_or_false",     ""},
    {"music_state",             ""},
    {"music_id",                ""},
    {"music_pos",               ""},
    {"study_state",             ""},
};
/*
csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                        "-",    // system_state (START/STOP/ERROR)
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
*/

class ofApp : public ofBaseApp{

	public:
        // MAIN WINDOW
        void setup();
		void update();
		void draw();
        void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
  
        // STUDY INPUT
        void seekFrameCallback(bool & val);
        void nbackNewCharCallback(std::string & val);
        void nbackCharHiddenCallback(bool & val);
        std::string nbackResultToString(int result);
  
        ofxSeekThermalGrabber seek;
        ofxNbackTest nback;
        ofImage _img;
        ofxRHUtilities::SimpleCsvLogger csv;
        std::vector<std::string> _csv_header;
        std::string _recording_path;
        unsigned int _seek_frame_number;
        bool _is_recording;
        
        // STUDY OUTPUT
        bool lookupFilesInDir(std::string target_path,
                              std::string extension,
                              std::vector<std::string> & file_list);
        
        // STUDY MANAGEMENT
        ofxRHUtilities::ThreadMap threadMap;
        std::thread _thread;
  
        // GUI WINDOW
        void setupGui();
        void drawGui(ofEventArgs & args);
        void exitGui(ofEventArgs & args);
        void nbackCallback(bool & val);
        void recordingCallback(bool & val);
        void killAppCallback(bool & val);
        
        ofParameterGroup parameters;
        ofParameter<bool> _nback_running;
        ofParameter<std::string> _file_path;
        ofParameter<std::string> _file_prefix;
        ofParameter<bool> _recording;
        ofParameter<bool> _kill_app;
        ofxPanel gui;
};
