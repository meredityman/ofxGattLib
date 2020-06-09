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

        virtual void onConnect(){ofLogNotice("Device::onConnect");};
;
	    void register_notification(const std::string &  uuid, shared_ptr<Device> device, gattlib_event_handler_t notification_handler);
	    void register_notification(uuid_t g_uuid, shared_ptr<Device> device, gattlib_event_handler_t notification_handler);

        void write(const std::string & uuid, const std::string & data);
        void write(uuid_t g_uuid, const char * data);

        vector<uint8_t> read(const std::string & uuid);
        vector<uint8_t> read(uint32_t uuid);
        vector<uint8_t> read(uuid_t uuid);

        static vector<uint8_t> moveBuffer( const uint8_t *buffer, size_t buffer_len);
        static void printBuffer(const vector<uint8_t> &  buffer, const std::string & msg);


    protected:
        // void notification_handler(const uuid_t* uuid, const uint8_t* data, size_t data_length, void* user_data);


        string addr;
        string name;
        bool bConnected;

        gatt_connection_t* gatt_connection;
        vector<gattlib_primary_service_t> services;
        vector<gattlib_characteristic_t>  characteristics;

    };

}