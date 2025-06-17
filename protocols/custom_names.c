#include "custom_names.h"
#include "../helpers/ble_payload.h"
#include <furi.h>
#include <stdlib.h>

#define CUSTOM_NAME "FlipperSpam"

bool send_custom_name() {
    BlePayload payload = {0};
    
    // Create BLE payload with custom name
    create_ble_payload(&payload, CUSTOM_NAME);
    
    // Send the payload
    bool result = ble_send_payload(&payload);
    
    // Clean up
    if(payload.data) {
        free(payload.data);
    }
    
    return result;
}

bool send_custom_name_with_text(const char* name) {
    if(!name) return false;
    
    BlePayload payload = {0};
    
    // Create BLE payload with provided name
    create_ble_payload(&payload, name);
    
    // Send the payload
    bool result = ble_send_payload(&payload);
    
    // Clean up
    if(payload.data) {
        free(payload.data);
    }
    
    return result;
}