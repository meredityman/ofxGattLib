#pragma once
#include "ofMain.h"
#include "device.h"


namespace ofxGattLib::Contrib {

    class Device_polarH10 : public Device {
    public:
        
        static const std::string UUID_PWD_CONTROL_POINT;
        static const std::string UUID_DATA_MTU_CHAR;

        enum CONTROL_POINT_ERROR_CODES_T {
            SUCCESS                        = 0,
            ERROR_INVALID_OP_CODE          = 1,
            ERROR_INVALID_MEASUREMENT_TYPE = 2,
            ERROR_NOT_SUPPORTED            = 3,
            ERROR_INVALID_LENGTH           = 4,
            ERROR_INVALID_PARAMETER        = 5,
            ERROR_INVALID_STATE            = 6,
            ERROR_INVALID_RESOLUTION       = 7,
            ERROR_INVALID_SAMPLE_RATE      = 8,
            ERROR_INVALID_G_RATE           = 9
        };

    
        Device_polarH10( string addr, string name) : 
            Device( addr, name) 
        {};


        virtual void onConnect() override;

        void onPWDControlPointNotify(const uuid_t* uuid, const uint8_t* data, size_t data_length, void* user_data);


    private: 

        void getSupportedChannels();
    };
}; 