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

// STUDY_SETUP
#define TRACKS_FOR_EACH_SESSION 3
#define SOUND_SAMPLING_RATE 44100
#define SOUND_BUFFER_SIZE 256

#define STUDY_INTRO_MESSAGE_MAIN "Stay there..."
#define STUDY_INTRO_MESSAGE_SUB "Just relax and wait for study starts (20mins)"
#define STUDY_CALIB_MESSAGE_MAIN "Again, stay there..."
#define STUDY_CALIB_MESSAGE_SUB "Camera calibration process is running, stay there. (1min)"
#define STUDY_PRACTICE_MESSAGE_MAIN "Practice session starts soon."
#define STUDY_PRACTICE_MESSAGE_SUB "Once the gauge in below is over, a practice session starts."
#define STUDY_MAIN_MESSAGE_MAIN "Main study starts soon."
#define STUDY_MAIN_MESSAGE_SUB "Once the gauge in below is over, a practice session starts."
#define STUDY_RUNNING_MESSAGE_MAIN "Shhh... listen to this."
#define STUDY_RUNNING_MESSAGE_SUB ""
#define STUDY_INTERVAL_MESSAGE_MAIN "Break time. Sit back and relax."
#define STUDY_INTERVAL_MESSAGE_SUB "Wait until the next session starts. (5mins)"
#define STUDY_END_MESSAGE_MAIN "Thanks for your participation!"
#define STUDY_END_MESSAGE_SUB "You can proceed to the questionnaire section."

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

/*
 * SCENES:
 * 0  - none
 * 1  - climate init (intro)
 * 2  - 1 minute calibration
 * 3  - practice intro
 * 4  - practice session
 * 5  - 5 minutes pause
 * 6  - main intro
 * 7  - LCLLV
 * 8  - 5 minutes pause
 * 9  - LCLHV
 * 10 - 5 minutes pause
 * 11 - HCLLV
 * 12 - 5 minutes pause
 * 13 - HCLHV
 * 14 - end
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
        
        // STUDY OUTPUT
        ofxRHUtilities::SimpleCsvLogger csv;
        std::vector<std::string> _csv_header;
        std::string _recording_path;
        unsigned int _seek_frame_number;
        bool _is_recording;
        
        // STUDY DISPLAY
        ofImage _img;
        ofTrueTypeFont font;
        std::string _study_scene_main_phrase;
        std::string _study_scene_sub_phrase;
        
        // STUDY MANAGEMENT
        void studyScenarioChangeCallback();
        void cancelBackgroundThread(const std::string & tname);
        ofxRHUtilities::ThreadMap threadMap;
        std::thread _thread;
        const std::string _thread_name = "WORKER_THREAD";
        unsigned _study_scenario_count;
        
        // AUDIO MANAGEMENT
        void audioOut(ofSoundBuffer & buffer);
        bool lookupFilesInDir(std::string target_path,
                              std::string extension,
                              std::vector<std::string> & file_list);
        ofSoundStream sound_stream;
        ofSoundPlayer player;
        std::vector<std::string> _hclhv_sounds;
        std::vector<std::string> _hcllv_sounds;
        std::vector<std::string> _lclhv_sounds;
        std::vector<std::string> _lcllv_sounds;
  
        // GUI WINDOW
        void setupGui();
        void drawGui(ofEventArgs & args);
        void exitGui(ofEventArgs & args);
        void nbackCallback(bool & val);
        void recordingCallback(bool & val);
        void killAppCallback(bool & val);
        void wholeStudyRunningCallback(bool & val);
        
        ofParameterGroup proto_parameters;
        ofParameter<bool> _nback_running;
        ofParameter<bool> _recording;
        ofParameter<bool> _kill_app;
        
        ofParameterGroup main_parameters;
        ofParameter<std::string> _participant_id;
        ofParameter<std::string> _file_path;
        ofParameter<std::string> _file_prefix;
        ofParameter<bool> _whole_study_running;
        
        ofxPanel gui;
};
