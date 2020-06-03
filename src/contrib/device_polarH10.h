#pragma once
#include "ofMain.h"
#include "device.h"


namespace ofxGattLib::Contrib {

    class Device_polarH10 : public Device {
    public:
    
        Device_polarH10( string addr, string name) : 
            Device( addr, name) 
        {};

    };

}; 