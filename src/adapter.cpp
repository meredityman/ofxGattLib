#include "adapter.h"

using namespace ofxGattLib;



GMainLoopContext::GMainLoopContext(){
    GMainLoopContext::m_main_loop = g_main_loop_new(NULL, 0);
    ofAddListener( ofEvents().update, this, &GMainLoopContext::onUpdate, OF_EVENT_ORDER_BEFORE_APP );
}

GMainLoopContext::~GMainLoopContext(){
    ofRemoveListener( ofEvents().update, this, &GMainLoopContext::onUpdate);
    g_main_loop_unref(m_main_loop);
}

void GMainLoopContext::onUpdate(ofEventArgs & args){

    GMainContext * m_main_loop_context = g_main_loop_get_context (m_main_loop);
    g_main_context_iteration(m_main_loop_context, false);
}


map<string, shared_ptr<Device>> Adapter::devices;

Adapter::Adapter():
    gMainLoopContext()
{
    int ret = gattlib_adapter_open(NULL, &adapter);

    if (ret == GATTLIB_SUCCESS) {
        ofLogNotice("ofxGattLib::Adapter") << "Succesfully opened default adapter.";
    } else {
        ofLogError("ofxGattLib::Adapter")  << "Failed to open adapter. Error: " << ret;
        return;
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

shared_ptr<Device> Adapter::connect(const string & addr){
	return connect<Device>(addr);
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