#pragma once

#include "ofMain.h"
#include "ofxSeekThermal.h"
#include "opencv2/opencv.hpp"
#include "ofxNbackTest.hpp"

#include "ofxGui.h"

#define ESEN_FONT_PATH "gt-pressura-regular.ttf"
#define RYO_FONT_PATH "/System/Library/Fonts/SFNS.ttf"
#define VERBOSE_MODE 1

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
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
  
        void seekFrameCallback(bool & val);
  
        ofxSeekThermalGrabber seek;
        ofxNbackTest nback;
        ofImage _img;
        ofImage _rawImg;
        std::string _recording_path;
        unsigned int _seek_frame_number;
        bool _is_recording;
  
        // GUI WINDOW
        void setupGui();
        void drawGui(ofEventArgs & args);
        void exitGui();
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
