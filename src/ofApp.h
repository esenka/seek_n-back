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
#define PRACTICE_PATH "stimuli/practice"
#define HCLHV_PATH "stimuli/HCLHV"
#define HCLLV_PATH "stimuli/HCLLV"
#define LCLHV_PATH "stimuli/LCLHV"
#define LCLLV_PATH "stimuli/LCLLV"
#define STUDY_ORDER_KEEPER_XML "order.xml"

// TIMING_SETUP
// to run software at the normal mode, comment three lines in below out
// and bring another three lines back active
// 4_DEBUG_PURPOSE_BEGIN
#define STUDY_INTRO_TIME 60000
#define STUDY_CALIB_TIME 60000
#define STUDY_INTVL_TIME 60000
// 4_DEBUG_PURPOSE_END

//#define STUDY_INTRO_TIME 600000     // 10 minutes in MS
//#define STUDY_CALIB_TIME 60000      // 1 minute in MS
//#define STUDY_INTVL_TIME 300000     // 5 minutes in MS

// STUDY_SETUP
#define TRACKS_FOR_EACH_SESSION 3
#define SOUND_SAMPLING_RATE 48000
#define SOUND_BUFFER_SIZE 1024

// the time for each round in Nback task can be calculated by
// NBACK_CHAR_TIME_MS + NBACK_BLANK_TIME_MS
#define NBACK_CHAR_TIME_MS 500
#define NBACK_BLANK_TIME_MS 2000
#define NBACK_RANDOMNESS_RATE 0.2
#define NBACK_COUNT_EASY 2
#define NBACK_COUNT_DIFF 4
#define NBACK_COUNT_PRAC 3

#define STUDY_INTRO_MESSAGE_MAIN "Stay there..."
#define STUDY_INTRO_MESSAGE_SUB "Just relax and wait for study starts (10mins)"
#define STUDY_CALIB_MESSAGE_MAIN "Again, stay there..."
#define STUDY_CALIB_MESSAGE_SUB "Camera calibration process is running, stay there. (1min)"
#define STUDY_PRACTICE_MESSAGE_MAIN "Practice session starts soon."
#define STUDY_PRACTICE_MESSAGE_SUB "Once the gauge in below is over, a practice session starts."
#define STUDY_MAIN_MESSAGE_MAIN "Main study starts soon."
#define STUDY_MAIN_MESSAGE_SUB "Once the gauge in below is over, a practice session starts."
#define STUDY_RUNNING_MESSAGE_MAIN "Shhh... listen to this."
#define STUDY_RUNNING_MESSAGE_SUB ""
#define STUDY_INTERVAL_MESSAGE_MAIN "Break time. Please go through the TLX study."
#define STUDY_INTERVAL_MESSAGE_SUB "then wait until the next session starts. (5mins)"
#define STUDY_END_MESSAGE_MAIN "Thanks for your participation!"
#define STUDY_END_MESSAGE_SUB "You can proceed to the questionnaire section."

// SYSTEM SETUP
#define VERBOSE_MODE 1	// comment this line out to suppress verbose output
#define DEBUG_GUI_ENABLE 1	// comment this line out to hide debug GUI options

static std::vector<std::string> _csv_header_only = {
    "system_timestamp_millis",
    "system_state",
    "study_state",
    "frame_file_name",
    "nback_state",
    "nback_character",
    "nback_response",
    "nback_true_or_false",
    "music_state",
    "music_id",
    "music_pos"
};
/*
csv.update<std::string>(std::to_string(ofGetSystemTimeMillis()),
                        "-",    // system_state (START/STOP/ERROR)
                        "-",    // study_state (START/STOP/CALIB/INTRO/PRACTICE/
                                //              INTVL/HCLHV/HCLLV/LCLHV/LCLLV)
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
		// seek thermal camera
        void seekNewFrameCallback(bool & val);
		ofxSeekThermalGrabber seek;
		ofImage _img;
		unsigned int _seek_frame_number;
  
        // webcam
        ofVideoGrabber video;
	
		// nback task
        void nbackNewCharCallback(std::string & val);
        void nbackCharHiddenCallback(bool & val);
        std::string nbackResultToString(int result);
		void nbackRunCallback(bool & val);
        ofxNbackTest nback;
        
        // STUDY OUTPUT
        ofxRHUtilities::SimpleCsvLogger csv;
        std::string _recording_path;
        bool _is_recording;
        
        // STUDY DISPLAY
		void drawStringCenterWithRatio(std::string message,
									   float center_x, float center_y,
									   float rate);
        void drawProgressBarCenterWithSize(float center_x, float center_y,
                                           float size_w, float size_h,
                                           float prev_time, float next_time);
        ofTrueTypeFont font;
        std::string _study_scene_main_phrase;
        std::string _study_scene_sub_phrase;
		float _study_phrase_sub_to_main_ratio;
        
        // STUDY MANAGEMENT
        void studyScenarioChangeCallback();
        void cancelBackgroundThread(const std::string & tname);
		void startWholeStudy();
		void stopWholeStudy();
		void audioFileManager();
        ofxRHUtilities::ThreadMap threadMap;
        std::thread _thread;
        const std::string _thread_name = "WORKER_THREAD";
        unsigned _study_scenario_count;
        std::vector<unsigned> _study_scenario_counts;
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
        unsigned _current_audio_pos;
        std::string _study_state;
        float _prev_progress_time;
        float _next_progress_time;
        ofXml studyOrderXml;
        
        // AUDIO MANAGEMENT
        void audioOut(ofSoundBuffer & buffer);
        bool lookupFilesInDir(std::string target_path,
                              std::string extension,
                              std::vector<std::string> & file_list,
							  bool clear_list_buffer);
        ofSoundStream sound_stream;
        ofSoundPlayer player;
		std::vector<std::string> _practice_sounds;
        std::vector<std::string> _hclhv_sounds;
        std::vector<std::string> _hcllv_sounds;
        std::vector<std::string> _lclhv_sounds;
        std::vector<std::string> _lcllv_sounds;
        std::string _current_sound_which;
        bool _b_music_already_playing;
  
        // GUI WINDOW
        void setupGui();
        void drawGui(ofEventArgs & args);
        void exitGui(ofEventArgs & args);
		void keyReleasedGui(ofKeyEventArgs & args);
        void recordingCallback(bool & val);
        void killAppCallback(bool & val);
        void wholeStudyRunCallback(bool & val);
        
		// PARAMETERS
        ofParameterGroup proto_parameters;
		ofParameter<bool> _nback_visible;
        ofParameter<bool> _nback_running;
        ofParameter<bool> _recording;
        
        ofParameterGroup main_parameters;
        ofParameter<std::string> _participant_id;
        ofParameter<std::string> _file_path;
        ofParameter<std::string> _file_prefix;
        ofParameter<bool> _whole_study_running;
		ofParameter<bool> _kill_app;
        
        ofxPanel gui;
};
