#pragma once
#include "ofMain.h"
#include "gattlib.h"


namespace ofxGattLib {

    class Device {
    public:

        Device( string addr, string name);
        virtual ~Device();

        bool connect();
        void disconnect();
        void discover_services();

        void write();
        void read(const string & uuid);
        void read(uint32_t uuid);
        void read(uuid_t uuid);


    protected:

        string addr;
        string name;
        bool bConnected;

        gatt_connection_t* gatt_connection;
        gattlib_primary_service_t* services;
        gattlib_characteristic_t* characteristics;
        int services_count, characteristics_count;
	    char uuid_str[MAX_LEN_UUID_STR + 1];
    };

}