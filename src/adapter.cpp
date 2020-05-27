#include "adapter.h"

using namespace ofxGattLib;


Device::Device( string addr, string name) :
	addr(addr),
	name(name),
	bConnected(false),
	gatt_connection(nullptr),
	services(nullptr),
	characteristics(nullptr)
{
}
		
Device::~Device(){
	disconnect();
}

bool Device::connect(){
	if(bConnected)
		disconnect();

	gatt_connection = gattlib_connect(NULL, addr.c_str(), GATTLIB_CONNECTION_OPTIONS_LEGACY_DEFAULT);
	if (gatt_connection == nullptr) {
		ofLogError("ofxGattLib::connect") << "Fail to connect to bluetooth device " << addr;
		return false;
	} else {
		ofLogNotice("ofxGattLib::connect") << "Connected to bluetooth device " << addr;
	}
	bConnected = true;	
	discover_services();
	return true;
}


void Device::discover_services(){
	int ret, i;
	ret = gattlib_discover_primary(gatt_connection, &services, &services_count);
	if (ret != GATTLIB_SUCCESS) {
		ofLogError("ofxGattLib::discover_services") << "Fail to discover primary services for " << addr;
		return;
	}

	for (i = 0; i < services_count; i++) {
		gattlib_uuid_to_string(&services[i].uuid, uuid_str, sizeof(uuid_str));

		printf("service[%d] start_handle:%02x end_handle:%02x uuid:%s\n", i,
				services[i].attr_handle_start, services[i].attr_handle_end,
				uuid_str);
	}

	ret = gattlib_discover_char(gatt_connection, &characteristics, &characteristics_count);
	if (ret != GATTLIB_SUCCESS) {
		ofLogError("ofxGattLib::discover_services") << "Fail to discover characteristics for " << addr;
		return;
	}
	for (i = 0; i < characteristics_count; i++) {
		gattlib_uuid_to_string(&characteristics[i].uuid, uuid_str, sizeof(uuid_str));

		printf("characteristic[%d] properties:%02x value_handle:%04x uuid:%s\n", i,
				characteristics[i].properties, characteristics[i].value_handle,
				uuid_str);
	}
}

void Device::disconnect(){
	int ret = gattlib_disconnect(gatt_connection);
	if (ret == GATTLIB_SUCCESS) {
        ofLogNotice("ofxGattLib::disconnect") << "Succesfully disconected from " << addr;
    } else {
        ofLogError("ofxGattLib::disconnect")   << "Succesfully disconected from " << addr << ". Error: " << ret;
    }

	if(gatt_connection != nullptr){
		free(gatt_connection);
		gatt_connection = nullptr;
	}

	if(services != nullptr){
		free(services);
		services = nullptr;
	}

	if(characteristics != nullptr){
		free(characteristics);
		characteristics = nullptr;
	}
	services_count = 0;
	characteristics_count = 0;
}

map<string, shared_ptr<Device>> Adapter::devices;

Adapter::Adapter(){
    int ret = gattlib_adapter_open(NULL, &adapter);

    if (ret == GATTLIB_SUCCESS) {
        ofLogNotice("ofxGattLib::Adapter") << "Succesfully opened default adapter.";
    } else {
        ofLogError("ofxGattLib::Adapter")  << "Failed to open adapter. Error: " << ret;
    }
}

Adapter::~Adapter(){
    gattlib_adapter_close(adapter);
}

void Adapter::scan(int timeout){
    devices.clear();

    int ret = gattlib_adapter_scan_enable(adapter, ble_discovered_device, timeout, NULL /* user_data */);

    if (ret == GATTLIB_SUCCESS) {
        ofLogVerbose("ofxGattLib::scan") << "Scan succeeded.";
    } else {
        ofLogError("ofxGattLib::scan")   << "Scan failed. Error: " << ret;
    }

    ret = gattlib_adapter_scan_disable(adapter);

    if (ret == GATTLIB_SUCCESS) {
        ofLogVerbose("ofxGattLib::scan") << "Scan disable succeeded.";
    } else {
        ofLogError("ofxGattLib::scan") <<  "Scan disable failed. Error: " << ret;
    }
}

bool Adapter::connect(const string & addr){
	auto device_itr = devices.find(addr);

	if(device_itr == devices.end()){
		auto result = devices.insert( 
			std::pair<string, shared_ptr<Device>>(
				addr, 
				std::make_shared<Device>(addr, "")
			)
		);

		if(result.second){
			device_itr = result.first;
		} else {
			exit(-1);
		}
	}

	return device_itr->second->connect();

}

void Adapter::ble_discovered_device(void *adapter, const char* addr, const char* name, void *user_data) {
    if (name) {
        ofLogNotice("ofxGattLib::Adapter") <<  "Discovered " << addr << " " << name;            
        devices.insert( 
			std::pair<string, shared_ptr<Device>>(
				addr, 
				std::make_shared<Device>(addr, "")
			)
		);

    } else {
        ofLogNotice("ofxGattLib::Adapter") <<  "Discovered " << addr;            
        devices.insert( 
			std::pair<string, shared_ptr<Device>>(
				addr, 
				std::make_shared<Device>(addr, "")
			)
		);
    }    
}