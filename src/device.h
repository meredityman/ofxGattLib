#pragma once
#include "ofMain.h"
#include "gattlib.h"


namespace ofxGattLib {
    class Device {
    public:
        typedef void (Device::*device_event_handler_t)(const uuid_t* uuid, const uint8_t* data, size_t data_length, void* user_data);
        typedef std::function<void (const uuid_t* uuid, const uint8_t* data, size_t data_length, void* user_data)> device_event_callback_t;


        Device( string addr, string name);
        virtual ~Device();

        bool connect();
        void disconnect();
        void discover_services();

        virtual void onConnect(){ofLogNotice("Device::onConnect");};

	    void register_notification(const std::string &  uuid,  device_event_handler_t notification_handler);
	    void register_notification(uuid_t g_uuid, device_event_handler_t notification_handler);

        void write(const std::string & uuid, const std::string & data);
        void write(uuid_t g_uuid, const char * data);

        vector<uint8_t> read(const std::string & uuid);
        vector<uint8_t> read(uint32_t uuid);
        vector<uint8_t> read(uuid_t uuid);

        static vector<uint8_t> moveBuffer( const uint8_t *buffer, size_t buffer_len);
        static void printBuffer(const vector<uint8_t> &  buffer, const std::string & msg);

        static void static_notification_handler(const uuid_t* uuid, const uint8_t* data, size_t data_length, void* user_data);
        void nonstatic_notification_handler(const uuid_t* uuid, const uint8_t* data, size_t data_length, void* user_data);

    protected:
        string addr;
        string name;
        bool bConnected;

        gatt_connection_t* gatt_connection;
        vector<gattlib_primary_service_t> services;
        vector<gattlib_characteristic_t>  characteristics;

    };
    
    class DeviceCallbacks{
        public:
        
        static DeviceCallbacks * getInstance(){
            if (instance == nullptr) {
                instance = new DeviceCallbacks();
            }
            return instance;
        }
        public:

        static void registerCallback(const uuid_t* uuid, Device::device_event_callback_t callback){
            auto instance = DeviceCallbacks::getInstance();
            char uuid_cstr[MAX_LEN_UUID_STR + 1];
            gattlib_uuid_to_string(uuid, uuid_cstr, sizeof(uuid_cstr));
            string uuid_str(uuid_cstr);

            instance->callbacks.insert({uuid_str, callback});
        };

        static void call(const uuid_t* uuid, const uint8_t* data, size_t data_length, void* user_data){
            auto instance = DeviceCallbacks::getInstance();

            char uuid_cstr[MAX_LEN_UUID_STR + 1];
            gattlib_uuid_to_string(uuid, uuid_cstr, sizeof(uuid_cstr));
            string uuid_str(uuid_cstr);


            auto result = instance->callbacks.equal_range(uuid_str);

            int count = std::distance(result.first, result.second);

            if(count == 0){

            } else {
                for (auto  it = result.first; it != result.second; it++){
                    it->second(uuid, data, data_length, user_data);
                }
            }
        }

        private:

        DeviceCallbacks(){};

        static DeviceCallbacks * instance ;
        std::multimap<std::string, Device::device_event_callback_t> callbacks;
    };
    

}