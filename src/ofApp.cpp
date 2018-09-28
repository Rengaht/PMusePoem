#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	_receiver.setup(PORT);	
	_img_muse.load("muse.png");
	
	ofRegisterURLNotification(this);

	_time_delay=0;
	_time_fadein=800;
	_time_show=1000;
	_time_fadeout=3000;

}

//--------------------------------------------------------------
void ofApp::update(){
	updateOsc();
}

void ofApp::updateOsc(){
	while(_receiver.hasWaitingMessages()){
		ofxOscMessage m;
		_receiver.getNextMessage(m);
		if(m.getAddress()=="/face"){
			int mood_=m.getArgAsFloat(0);
			ofLog()<<"get face!";
			_str_info="get face!!";
			sendPoemRequest(mood_);
		}
	}
}

void ofApp::sendPoemRequest(float mood_){
	int id=ofLoadURLAsync("http://muse.mmlab.com.tw:5000/generate/mood/"+ofToString(mood_));
	_str_info+="Request Poem, mood="+ofToString(mood_)+"/n";
	ofLog()<<_str_info;
}
void ofApp::urlResponse(ofHttpResponse& resp){
	string data=resp.data;
	_str_info+="Receive data: "+data;
	ofLog()<<_str_info;

	auto s=ofSplitString(data,"|");
	int len=s.size();

	sendOsc(concatPoem(s,0,min(len,4)),"192.168.2.172",_time_delay,_time_fadein*4,_time_show+_time_fadein*3,_time_fadeout);
	if(len>4){
		sendOsc(concatPoem(s,4,min(len,6)),"192.168.2.115",_time_delay+_time_fadein*4,_time_fadein*2,_time_show+_time_fadein,_time_fadeout);
		if(len>6) sendOsc(concatPoem(s,6,len),"192.168.2.171",_time_delay+_time_fadein*6,_time_fadein,_time_show,_time_fadeout);
	}

}
string ofApp::concatPoem(vector<string> list_,int begin_,int end_){
	string s_="";
	for(int i=begin_;i<end_;++i) s_+=list_[i]+"|";
	return s_;
}

void ofApp::sendOsc(string str_,string ip_,int tdelay_,int tin_,int tshow_,int tout_){

	_str_info+="send poem to "+ip_+"\n";
	ofLog()<<_str_info;
	
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
//--------------------------------------------------------------
void ofApp::draw(){
	ofSetBackgroundColor(0);
	float wid=ofGetWidth()*.6;
	float hei=wid*0.33;

	ofPushMatrix();
	ofTranslate(ofGetWidth()/2-wid/2,ofGetHeight()/2-hei/2);
		_img_muse.draw(0,0,wid,hei);
	ofPopMatrix();

	ofPushStyle();
	ofSetColor(255,0,0);
	ofDrawBitmapString(_str_info,0,ofGetHeight()/2);
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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
