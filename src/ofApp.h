#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"
#define PORT 12345


class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		ofImage _img_muse;
		ofxOscReceiver _receiver;
		void updateOsc();
		void sendPoemRequest(float mood_);
		void urlResponse(ofHttpResponse& response);

		string concatPoem(vector<string> list_,int begin_,int end_);

		int _time_delay,_time_fadein,_time_show,_time_fadeout;
		void sendOsc(string str_,string ip_,int tdelay_,int tin_,int tshow_,int tout_);

		string _str_info;

};
