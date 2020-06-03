#pragma once
#include "ofMain.h"
#include "gattlib.h"
#include "device.h"


#define BLE_SCAN_TIMEOUT   4

namespace ofxGattLib {
    class Adapter {
        typedef void (*ble_discovered_device_t)(const char* addr, const char* name);

    public:

        Adapter();
        ~Adapter();

        void scan(int timeout = BLE_SCAN_TIMEOUT);


        
        template<class Device_t>
        shared_ptr<Device_t> connect(const string & addr);
        shared_ptr<Device> connect(const string & addr);

        static map<string, shared_ptr<Device>> devices;

    private: 
        void* adapter;


        static void ble_discovered_device(void *adapter, const char* addr, const char* name, void *user_data);

    };

    template<class Device_t> 
    shared_ptr<Device_t> Adapter::connect(const string & addr){
        auto device_itr = devices.find(addr);

        auto result = devices.insert( 
            device_itr,
            std::pair<string, shared_ptr<Device>>(
                addr, 
                std::make_shared<Device_t>(addr, "")
            )
        );

        // if(result.second){
        //     device_itr = result.first;
        // } else {
        //     exit(-1);
        // }

        device_itr->second->connect();
        return std::dynamic_pointer_cast<Device_t>(device_itr->second);
    }

};