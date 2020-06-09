#pragma once
#include "ofMain.h"
#include <glib.h>

#include "gattlib.h"
#include "device.h"


#define BLE_SCAN_TIMEOUT   4

namespace ofxGattLib {

    class GMainLoopContext {
        public:

        GMainLoopContext();
        ~GMainLoopContext();

        private:
            void onUpdate(ofEventArgs & args);

            GMainLoop *m_main_loop;
    };


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
        GMainLoopContext gMainLoopContext;

        static void ble_discovered_device(void *adapter, const char* addr, const char* name, void *user_data);

    };  

    template<class Device_t> 
    shared_ptr<Device_t> Adapter::connect(const string & addr){
        std::pair<std::map<string, shared_ptr<Device>>::iterator,bool> result = devices.insert( 
            std::pair<string, shared_ptr<Device>>(
                addr, 
                std::make_shared<Device_t>(addr, "")
            )
        );

        if(!result.second){
            result.first->second = std::make_shared<Device_t>(addr, "");
        }

        shared_ptr<Device_t> device = std::dynamic_pointer_cast<Device_t>(result.first->second);

        device->connect();
        return device;
    }

};