#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofSetWindowShape(800,480);
	ofHideCursor();
	
	
	_serial.setup("/dev/ttyAMA0",9600);	
	loadXmlSetting();

	_receiver.setup(PORT);	
	_img_muse.load("muse.png");
	
	ofRegisterURLNotification(this);	
	//ofAddListener(_http.newResponseEvent,this,&ofApp::httpResponse);

	_time_delay=0;
	_time_fadein=800;
	_time_show=1000;
	_time_fadeout=3000;
	
	float wid=ofGetWidth();
	float hei=ofGetHeight();

	_shader_blur.load("shadersES2/shaderBlurX");
	_shader_glitch.load("shadersES2/glitch");

	_shader_blur.begin();
	_shader_blur.setUniform1f("windowWidth",wid);
	_shader_blur.setUniform1f("windowHeight",hei);
	_shader_blur.end();
	
	_shader_glitch.begin();
	_shader_glitch.setUniform1f("windowWidth",wid);
	_shader_glitch.setUniform1f("windowHeight",hei);
	_shader_glitch.end();

	_fbo1.allocate(wid,hei);
	_fbo2.allocate(wid,hei);

	setStatus(PStatus::SLEEP);
}

//--------------------------------------------------------------
void ofApp::update(){
	updateOsc();
}

void ofApp::updateOsc(){
	while(_receiver.hasWaitingMessages()){
		ofxOscMessage m;
		_receiver.getNextMessage(m);
		string addr=m.getAddress();
		ofLog()<<"get message: "<<addr;
		if(addr=="/face"){
			int mood_=m.getArgAsFloat(0);
			ofLog()<<"get face!";
			_str_info+="get face!!";
			sendPoemRequest(mood_);
			setStatus(PStatus::PROCESS);

		}else if(addr=="/reset"){
			ofLog()<<"reset!";
			setStatus(PStatus::SLEEP);
		}
	}
}

void ofApp::sendPoemRequest(float mood_){
	string url_="http://muse.mmlab.com.tw:5000/generate/mood/"+ofToString(mood_);
	int id=ofLoadURLAsync(url_);
	
/*	ofxHttpForm form;
	form.method=OFX_HTTP_GET;
	_http.addForm(form);*/
	
	_str_info+="Request Poem, mood="+ofToString(mood_)+"/n";
	ofLog()<<_str_info;
}
void ofApp::urlResponse(ofHttpResponse& resp){
        string data=resp.data;
        _str_info+="Receive data: "+data;
        ofLog()<<"receive data";

        auto s=ofSplitString(data,"|");
        int len=s.size();

        sendOsc(concatPoem(s,0,min(len,4)),_str_ip[2],_time_delay,_time_fadein*4,_time_show+_time_fadein*3,_time_fadeout);
        if(len>4){
                sendOsc(concatPoem(s,4,min(len,6)),_str_ip[3],_time_delay+_time_fadein*4,_time_fadein*2,_time_show+_time_fadein,_time_fadeout);
                if(len>6) sendOsc(concatPoem(s,6,len),_str_ip[4],_time_delay+_time_fadein*6,_time_fadein,_time_show,_time_fadeout);
        }
        setStatus(PStatus::POEM);

}
/*void ofApp::httpResponse(ofxHttpResponse& resp){
	string data=(string)resp.responseBody;
	_str_info+="Receive data: "+data;
	ofLog()<<"receive data";

	auto s=ofSplitString(data,"|");
	int len=s.size();
	
	sendOsc(concatPoem(s,0,min(len,4)),_str_ip[2],_time_delay,_time_fadein*4,_time_show+_time_fadein*3,_time_fadeout);
	if(len>4){
		sendOsc(concatPoem(s,4,min(len,6)),_str_ip[3],_time_delay+_time_fadein*4,_time_fadein*2,_time_show+_time_fadein,_time_fadeout);
		if(len>6) sendOsc(concatPoem(s,6,len),_str_ip[4],_time_delay+_time_fadein*6,_time_fadein,_time_show,_time_fadeout);
	}
	setStatus(PStatus::POEM);

}*/

string ofApp::concatPoem(vector<string> list_,int begin_,int end_){
	string s_="";
	for(int i=begin_;i<end_;++i) s_+=list_[i]+"|";
	return s_;
}

void ofApp::sendOsc(string str_,string ip_,int tdelay_,int tin_,int tshow_,int tout_){

	_str_info+="send poem to "+ip_+"\n";
	ofLog()<<"send osc";
	
	ofxOscSender sender_;
	sender_.setup(ip_,PORT);
	
	ofxOscMessage message_;
	message_.setAddress("/poem");
	ofBuffer buf_;
	buf_.set(str_.c_str(),str_.size());
	message_.addBlobArg(buf_);

	message_.addIntArg(tdelay_);
	message_.addIntArg(tin_);
	message_.addIntArg(tshow_);
	message_.addIntArg(tout_);

	sender_.sendMessage(message_);

}
void ofApp::setStatus(PStatus set_){
	_status=set_;
	switch(_status){
		case SLEEP:
			_serial.writeByte('A');
			_str_info="";
			break;
		case PROCESS:
			_serial.writeByte('B');
			break;
		case POEM:
			_serial.writeByte('C');
			break;
	}

}


//--------------------------------------------------------------
void ofApp::draw(){
	ofSetBackgroundColor(0);

	float wid=ofGetWidth()*.6;
	float hei=wid*0.33;

	_fbo2.begin();
	ofClear(0);
	ofPushMatrix();
	ofTranslate(ofGetWidth()/2-wid/2,ofGetHeight()/2-hei/2);
		_img_muse.draw(0,0,wid,hei);
	ofPopMatrix();
	_fbo2.end();
	
	_fbo1.begin();
	ofClear(0);
	_shader_glitch.begin();
	_shader_glitch.setUniform1f("amount",10);
	_shader_glitch.setUniform1f("phi",ofRandom(-100,100));
	_shader_glitch.setUniform1f("angle",PI*sin(ofGetFrameNum()%40/40+(ofRandom(20)<1?0:ofRandom(-10,10))));
	_shader_glitch.setUniform1f("windowWidth",ofGetWidth());
	_shader_glitch.setUniform1f("windowHeight",ofGetHeight());
	_shader_glitch.setUniformTexture("tex0",_fbo2.getTexture(),0);
		_fbo2.draw(0,0);
	_shader_glitch.end();
	_fbo1.end();
	
	_fbo2.begin();
	ofClear(0);
	_shader_blur.begin();
	_shader_blur.setUniformTexture("tex0",_fbo1.getTexture(),0);
	_shader_blur.setUniform1f("windowWidth",ofGetWidth());
	_shader_blur.setUniform1f("windowHeight",ofGetHeight());
		_fbo1.draw(0,0);
	_shader_blur.end();
	_fbo2.end();

	_fbo1.begin();
	ofClear(0);
	_shader_blur.begin();
	_shader_blur.setUniformTexture("tex0",_fbo2.getTexture(),0);
		_fbo2.draw(0,0);	
	_shader_blur.end();
	_fbo1.end();
	
	_fbo1.draw(0,0);

	ofPushStyle();
	ofSetColor(255);
	ofDrawBitmapString(ofToString(ofGetFrameRate()),0,10);
	ofDrawBitmapString(_str_info,0,ofGetHeight()/3);
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch(key){
		case 'r':
		case 'R':
			setStatus(PStatus::SLEEP);
			break;
	}
}

void ofApp::loadXmlSetting(){
	ofxXmlSettings param_;
	param_.load("Pdata.xml");
	_str_ip.push_back(param_.getValue("IP_FACE",""));
	_str_ip.push_back(param_.getValue("IP_POEM",""));
	_str_ip.push_back(param_.getValue("IP_DISPLAY1",""));
	_str_ip.push_back(param_.getValue("IP_DISPLAY2",""));
	_str_ip.push_back(param_.getValue("IP_DISPLAY3",""));

}

