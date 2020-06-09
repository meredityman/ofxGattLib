#pragma once
#include "gattlib.h"


namespace ofxGattLib{

static string getCharPropertyString(uint8_t properties){
    switch(properties){
        case GATTLIB_CHARACTERISTIC_BROADCAST:
            return "Broadcast";
        case GATTLIB_CHARACTERISTIC_READ:
            return "Read";
        case GATTLIB_CHARACTERISTIC_WRITE_WITHOUT_RESP:
            return "Write without response";
        case GATTLIB_CHARACTERISTIC_WRITE:
            return "Write";
        case GATTLIB_CHARACTERISTIC_NOTIFY:
            return "Notify";
        case GATTLIB_CHARACTERISTIC_INDICATE:
            return "Indicate";
        default:
            size_t size = snprintf( nullptr, 0, "(%02x)", properties ) + 1;
            if( size <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
            std::unique_ptr<char[]> buf( new char[ size ] ); 
            snprintf( buf.get(), size, "(%02x)", properties );
            return "Unknown " + std::string( buf.get(), buf.get() + size - 1 );
    }
}

}

