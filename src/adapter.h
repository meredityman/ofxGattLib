#pragma once
#include "ofMain.h"
#include "gattlib.h"



#define BLE_SCAN_TIMEOUT   4

namespace ofxGattLib {

    class Device {
    public:

        Device( string addr, string name);
        ~Device();

        bool connect();
        void disconnect();
        void discover_services();

    private:

        string addr;
        string name;
        bool bConnected;

        gatt_connection_t* gatt_connection;
        gattlib_primary_service_t* services;
        gattlib_characteristic_t* characteristics;
        int services_count, characteristics_count;
	    char uuid_str[MAX_LEN_UUID_STR + 1];
    };


    class Adapter {
        typedef void (*ble_discovered_device_t)(const char* addr, const char* name);

    public:

        Adapter();
        ~Adapter();

        void scan(int timeout = BLE_SCAN_TIMEOUT);

        bool connect(const string & addr);

        static map<string, shared_ptr<Device>> devices;

    private: 
        void* adapter;


        static void ble_discovered_device(void *adapter, const char* addr, const char* name, void *user_data);

    };

    

};