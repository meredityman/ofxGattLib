#include "ofApp.h"

using namespace ofxGattLib;

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);

    adapter.scan();

    auto device = adapter.connect("E8:6E:CA:B5:6B:90");

	const uuid_t g_battery_level_uuid{
		SDP_UUID16, 
		(0x2A19)
	};

	const uuid_t g_serial_number_uuid{
		SDP_UUID16, 
		(0x2A24)
	};



    //device->read(g_serial_number_uuid);
    device->read("0000180f-0000-1000-8000-00805f9b34fb");


    device->read("00002A19-0000-1000-8000-00805F9B34FB");


    device->write("FB005C81-02E7-F387-1CAD-8ACD2D8DF0C8", "0x1234");

    device->read("FB005C81-02E7-F387-1CAD-8ACD2D8DF0C8");
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

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
