#include "device.h"

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


void Device::disconnect(){
	if(!bConnected)
		return;

	int ret = gattlib_disconnect(gatt_connection);
	if (ret == GATTLIB_SUCCESS) {
        ofLogNotice("ofxGattLib::disconnect") << "Succesfully disconected from " << addr;
    } else {
        ofLogError("ofxGattLib::disconnect")   << "Succesfully disconected from " << addr << ". Error: " << ret;
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

void Device::write(){
    // ret = gattlib_write_char_by_uuid(connection, &g_uuid, &value_data, sizeof(value_data));
    // if (ret != GATTLIB_SUCCESS) {
    //     char uuid_str[MAX_LEN_UUID_STR + 1];

    //     gattlib_uuid_to_string(&g_uuid, uuid_str, sizeof(uuid_str));

    //     if (ret == GATTLIB_NOT_FOUND) {
    //         fprintf(stderr, "Could not find GATT Characteristic with UUID %s. "
    //             "You might call the program with '--gatt-discovery'.\n", uuid_str);
    //     } else {
    //         fprintf(stderr, "Error while writing GATT Characteristic with UUID %s (ret:%d)\n",
    //             uuid_str, ret);
    //     }
    //     goto EXIT;
    // }
}

void Device::read(uuid_t g_uuid){
    if(!bConnected){
        ofLogError("ofxGattLib::read") << "Device not connected: " << name;
    }

    uint8_t *buffer = NULL;
    size_t len;

    int ret = gattlib_read_char_by_uuid(gatt_connection, &g_uuid, (void **)&buffer, &len);
    if (ret != GATTLIB_SUCCESS) {
        char uuid_str[MAX_LEN_UUID_STR + 1];

        gattlib_uuid_to_string(&g_uuid, uuid_str, sizeof(uuid_str));

        if (ret == GATTLIB_NOT_FOUND) {
            fprintf(stderr, "Could not find GATT Characteristic with UUID %s. "
                "You might call the program with '--gatt-discovery'.\n", uuid_str);
        } else {
            fprintf(stderr, "Error while reading GATT Characteristic with UUID %s (ret:%d)\n", uuid_str, ret);
        }
        return;
    }


    printf("Read UUID completed: ");
    for (int i = 0; i < len; i++) {
        printf("%02x ", buffer[i]);
    }
    printf("\n");

    free(buffer);
};

void Device::read(uint32_t uuid){
    uuid_t g_uuid;
    memcpy(&g_uuid.value, &uuid, sizeof(uuid));
    g_uuid.type = SDP_UUID32;

    read(g_uuid);
}

void Device::read(const string & uuid){
    uuid_t g_uuid;
	if (gattlib_string_to_uuid(uuid.c_str(), uuid.size(), &g_uuid) < 0) {
		ofLogError("ofxGattLib::read") << "Failed to convert UUID: " << uuid;
		return;
	}
    read(g_uuid);
}
