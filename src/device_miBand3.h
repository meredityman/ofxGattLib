#pragma once
#include "ofMain.h"
#include "device.h"


namespace ofxGattLib::Contrib {

    class Device_miBand3 : public Device {
    public:
    
        Device_miBand3( string addr, string name) : 
            Device( addr, name) 
        {};

    };

}; 