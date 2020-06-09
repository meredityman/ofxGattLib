#include "device_polarH10.h"


using namespace ofxGattLib::Contrib;



const std::string Device_polarH10::UUID_PWD_CONTROL_POINT{
    "FB005C81-02E7-F387-1CAD-8ACD2D8DF0C8"};

const std::string Device_polarH10::UUID_DATA_MTU_CHAR{
    "FB005C81-02E7-F387-1CAD-8ACD2D8DF0C8"};


void Device_polarH10::onConnect() {

    register_notification(UUID_PWD_CONTROL_POINT);

    getSupportedChannels();

    string data = "0x0102";
    write(UUID_PWD_CONTROL_POINT, data.c_str());
        
}

void Device_polarH10::getSupportedChannels(){
    auto resp = read(UUID_PWD_CONTROL_POINT);
    resp = vector<uint8_t>(resp.begin(), resp.begin() + 3);
    
    if(resp[0] == 0x0F ){
        ofLogNotice("Device_polarH10::onConnect") << "Control point feature read response recieved";

        if((resp[1] >> 0) & 1){
            ofLogNotice("Device_polarH10::onConnect") << "ecg_supported";
        }
        if((resp[1] >> 1) & 1){
            ofLogNotice("Device_polarH10::onConnect") << "ppg_supported";
        }
        if((resp[1] >> 2) & 1){
            ofLogNotice("Device_polarH10::onConnect") << "acc_supported";
        }
        if((resp[1] >> 3) & 1){
            ofLogNotice("Device_polarH10::onConnect") << "ppi_supported";
        }
    } else {
        ofLogNotice("Device_polarH10::onConnect") << "Control point feature read response error";
    }
}