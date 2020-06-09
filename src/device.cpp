#include "device.h"
#include "utils.h"

using namespace ofxGattLib;


Device::Device( string addr, string name) :
	addr(addr),
	name(name),
	bConnected(false),
	gatt_connection(nullptr)
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

	onConnect();
	return bConnected;
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
}

void Device::discover_services(){
	int ret, i;
	int services_count, characteristics_count;
    gattlib_primary_service_t* _services;
    gattlib_characteristic_t*  _characteristics;

	char uuid_str[MAX_LEN_UUID_STR + 1];
	
	ret = gattlib_discover_primary(gatt_connection, &_services, &services_count);
	if (ret != GATTLIB_SUCCESS) {
		ofLogError("ofxGattLib::discover_services") << "Fail to discover primary services for " << addr;
		return;
	}

	services.clear();
	for (i = 0; i < services_count; i++) {
		gattlib_uuid_to_string(&_services[i].uuid, uuid_str, sizeof(uuid_str));

		printf("service[%d] start_handle:%02x end_handle:%02x uuid:%s\n", i,
				_services[i].attr_handle_start, _services[i].attr_handle_end,
				uuid_str);

		services.push_back(_services[i]);
	}
	free(_services);

	characteristics.clear();
	ret = gattlib_discover_char(gatt_connection, &_characteristics, &characteristics_count);
	if (ret != GATTLIB_SUCCESS) {
		ofLogError("ofxGattLib::discover_services") << "Fail to discover characteristics for " << addr;
		return;
	}
	for (i = 0; i < characteristics_count; i++) {
		gattlib_uuid_to_string(&_characteristics[i].uuid, uuid_str, sizeof(uuid_str));

     	characteristics.push_back(_characteristics[i]);

		//_characteristics[i].value_handle
		ofLogNotice() << getCharPropertyString(_characteristics[i].properties) << ", " << uuid_str;
	}
	free(_characteristics);
}



void default_notification_handler(const uuid_t* uuid, const uint8_t* data, size_t data_length, void* user_data) {
	vector<uint8_t> values = Device::moveBuffer(data, data_length);
	Device::printBuffer(values, "Notification not handled");
}

void Device::register_notification(uuid_t g_uuid, shared_ptr<Device> device, gattlib_event_handler_t notification_handler){
	gattlib_register_notification(gatt_connection, default_notification_handler, NULL);

	int ret = gattlib_notification_start(gatt_connection, &g_uuid);
	if (ret != GATTLIB_SUCCESS) {
        char uuid_str[MAX_LEN_UUID_STR + 1];
        gattlib_uuid_to_string(&g_uuid, uuid_str, sizeof(uuid_str));

		ofLogError("Device::register_notification") << "Fail to start notification. Error " <<  uuid_str;
	} else {
		ofLogNotice("Device::register_notification") << "Notification registered";		
	};
}

void Device::register_notification(const std::string &  uuid, shared_ptr<Device> device, gattlib_event_handler_t notification_handler){
    uuid_t g_uuid;
	if (gattlib_string_to_uuid(uuid.c_str(), uuid.size(), &g_uuid) < 0) {
		ofLogError("ofxGattLib::read") << "Failed to convert UUID: " << uuid;
		return;
	}
	return register_notification(g_uuid, device, notification_handler);
}

void Device::write(const std::string & uuid, const std::string & data){
    uuid_t g_uuid;
	if (gattlib_string_to_uuid(uuid.c_str(), uuid.size(), &g_uuid) < 0) {
		ofLogError("ofxGattLib::write") << "Failed to convert UUID: " << uuid;
		return;
	}

	return write(g_uuid, data.c_str());
}

void Device::write(uuid_t g_uuid, const char * data){
    if(!bConnected){
        ofLogError("ofxGattLib::read") << "Device not connected: " << name;
		return;
    }

	long int value_data;

	if ((strlen(data) >= 2) && (data[0] == '0') && ((data[1] == 'x') || (data[1] == 'X'))) {
		value_data = strtol(data, NULL, 16);
	} else {
		value_data = strtol(data, NULL, 0);
	}
	printf("Value to write: 0x%lx\n", value_data);

    int ret = gattlib_write_char_by_uuid(gatt_connection, &g_uuid, &value_data, sizeof(value_data));
    if (ret != GATTLIB_SUCCESS) {
        char uuid_str[MAX_LEN_UUID_STR + 1];

        gattlib_uuid_to_string(&g_uuid, uuid_str, sizeof(uuid_str));

        if (ret == GATTLIB_NOT_FOUND) {
            fprintf(stderr, "Could not find GATT Characteristic with UUID %s. "
                "You might call the program with '--gatt-discovery'.\n", uuid_str);
        } else {
            fprintf(stderr, "Error while writing GATT Characteristic with UUID %s (ret:%d)\n",
                uuid_str, ret);
        }
    }
}

vector<uint8_t> Device::read(uuid_t g_uuid){
    if(!bConnected){
        ofLogError("ofxGattLib::read") << "Device not connected: " << name;
		return vector<uint8_t>();
    }


	bool bCharacteristicExists = false;
	for(auto & characteristic : characteristics ){
		if( gattlib_uuid_cmp( &characteristic.uuid, &g_uuid)){
			bCharacteristicExists = true;
			break;
		}
	}
	if(!bCharacteristicExists){
		char uuid_str[MAX_LEN_UUID_STR + 1];
        gattlib_uuid_to_string(&g_uuid, uuid_str, sizeof(uuid_str));
        ofLogWarning("ofxGattLib::read") << "GATT Characteristic with UUID " << uuid_str  << " may not exist.";
	}
    uint8_t *buffer = nullptr;
    size_t buffer_len;

    int ret = gattlib_read_char_by_uuid(gatt_connection, &g_uuid,  reinterpret_cast<void**>(&buffer), &buffer_len);
    if (ret != GATTLIB_SUCCESS) {
        char uuid_str[MAX_LEN_UUID_STR + 1];

        gattlib_uuid_to_string(&g_uuid, uuid_str, sizeof(uuid_str));

        if (ret == GATTLIB_NOT_FOUND) {
            ofLogError("ofxGattLib::read") << "Could not find GATT Characteristic with UUID " << uuid_str ;
        } else {
            ofLogError("ofxGattLib::read") << "Error while reading GATT Characteristic with UUID " << uuid_str << "(ret:" <<  ret << ")"; 
        }
		
		return vector<uint8_t>();
    }

	vector<uint8_t> values = moveBuffer(buffer, buffer_len);
	free(buffer);

	printBuffer(values, "Read UUID completed");
	return values;
};


vector<uint8_t> Device::read(uint32_t uuid){
    uuid_t g_uuid;
    memcpy(&g_uuid.value, &uuid, sizeof(uuid));
    g_uuid.type = SDP_UUID32;

    return read(g_uuid);
}

vector<uint8_t> Device::read(const string & uuid){
    uuid_t g_uuid;
	if (gattlib_string_to_uuid(uuid.c_str(), uuid.size(), &g_uuid) < 0) {
		ofLogError("ofxGattLib::read") << "Failed to convert UUID: " << uuid;
		return vector<uint8_t>();
	}
    return read(g_uuid);
}

vector<uint8_t> Device::moveBuffer( const uint8_t *buffer, size_t buffer_len){
	if(buffer_len == 1){
		ofLogNotice() << *buffer;
		return vector<uint8_t>();
	} else {
		return vector<uint8_t> (buffer, buffer + buffer_len);
	}

}

void Device::printBuffer(const vector<uint8_t> & buffer, const std::string & msg){
	ostringstream str;
    for (const auto val : buffer ) {
		size_t size = snprintf( nullptr, 0, "%02x ", val ) + 1;
		if( size <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
		std::unique_ptr<char[]> buf( new char[ size ] ); 
		snprintf( buf.get(), size, "%02x ", val );
		str <<  std::string( buf.get(), buf.get() + size - 1 ); 
    }

	ofLogNotice("ofxGattLib::read") << msg << "\n" << str.str();
}
