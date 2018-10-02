#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"
//#include "ofxHttpUtils.h"
#define PORT 12345


class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();		
		void keyPressed(int key);
		
		ofImage _img_muse;
		ofxOscReceiver _receiver;
		void updateOsc();
		void sendPoemRequest(float mood_);
		void urlResponse(ofHttpResponse& response);

		string concatPoem(vector<string> list_,int begin_,int end_);

		int _time_delay,_time_fadein,_time_show,_time_fadeout;
		void sendOsc(string str_,string ip_,int tdelay_,int tin_,int tshow_,int tout_);
		
		enum PStatus{SLEEP,PROCESS,POEM};
		PStatus _status;
		void setStatus(PStatus set_);

		string _str_info;

		ofShader _shader_blur,_shader_glitch;
		ofFbo _fbo1,_fbo2;

		vector<string> _str_ip;
		void loadXmlSetting();

		ofSerial _serial;

//		ofxHttpUtils _http;
//		void httpResponse(ofxHttpResponse& resp);

};
