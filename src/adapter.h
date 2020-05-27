#pragma once
#include "ofMain.h"
#include "gattlib.h"



#define BLE_SCAN_TIMEOUT   4

namespace ofxGattLib {

    struct connection_t {
        char* addr;
        connection_t(char* addr):
            addr(addr){}
    };

    class Adapter {
        typedef void (*ble_discovered_device_t)(const char* addr, const char* name);

    public:

        static vector<connection_t> connections;

        Adapter(){
            const char* adapter_name_c = NULL;
            status = gattlib_adapter_open(adapter_name_c, &adapter);

            if (status) {
                ofLogError("ofxGattLib::Adapter") <<  "Failed to open adapter.";
            } else {
                ofLogNotice("ofxGattLib::Adapter") <<  "Opened adapter [" << adapter_name_c << "]";
            }
        }

        void Scan(){
	        status = gattlib_adapter_scan_enable(adapter, ble_discovered_device, BLE_SCAN_TIMEOUT, NULL /* user_data */);
        
            if (status) {
                ofLogError("ofxGattLib::Scan") <<  "Scan failed!";
            } else {
                ofLogNotice("ofxGattLib::Scan") << "Scan succeeded.";
            }

            gattlib_adapter_scan_disable(adapter);
        
        }

    private: 
        void* adapter;
        int status;

        

        static void ble_discovered_device(void *adapter, const char* addr, const char* name, void *user_data) {
            int ret;

            if (name) {
                ofLogNotice("ofxGattLib::Adapter") <<  "Discovered " << addr << " " << name;
            } else {
                ofLogNotice("ofxGattLib::Adapter") <<  "Discovered " << addr;
            }

            connections.push_back(connection_t(strdup(addr)));
            
        }

    };

    

};