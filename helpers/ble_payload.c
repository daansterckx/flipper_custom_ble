#include "ble_payload.h"
#include <furi_hal_bt.h>
#include <furi.h>
#include <string.h>

void create_ble_payload(BlePayload* payload, const char* custom_name) {
    if(!payload || !custom_name) return;
    
    // Free existing data if any
    if(payload->data) {
        free(payload->data);
        payload->data = NULL;
    }
    
    // Create a basic BLE advertisement packet with custom name
    uint8_t* data = malloc(31); // Max BLE advertisement size
    if(!data) {
        payload->length = 0;
        return;
    }
    
    size_t name_len = strlen(custom_name);
    if(name_len > 25) name_len = 25; // Leave room for headers
    
    size_t offset = 0;
    
    // Flags
    data[offset++] = 0x02; // Length
    data[offset++] = 0x01; // Type: Flags
    data[offset++] = 0x06; // Value: LE General Discoverable Mode + BR/EDR Not Supported
    
    // Complete Local Name
    data[offset++] = name_len + 1; // Length
    data[offset++] = 0x09; // Type: Complete Local Name
    memcpy(&data[offset], custom_name, name_len);
    offset += name_len;
    
    payload->data = data;
    payload->length = offset;
}

bool ble_send_payload(BlePayload* payload) {
    // Guard against NULL or invalid payload
    if(!payload || !payload->data || payload->length == 0) {
        FURI_LOG_E("BLE_PAYLOAD", "Invalid payload");
        return false;
    }
    
    // Safety check on payload length
    if(payload->length > 31) {  // BLE max adv data size
        FURI_LOG_E("BLE_PAYLOAD", "Payload too large: %d bytes", payload->length);
        return false;
    }
    
    // Set beacon data - no need for critical sections here
    bool result = furi_hal_bt_extra_beacon_set_data(payload->data, payload->length);
    
    if(result) {
        // Start beacon transmission
        result = furi_hal_bt_extra_beacon_start();
    } else {
        FURI_LOG_E("BLE_PAYLOAD", "Failed to set beacon data");
    }
    
    return result;
}

void ble_stop_payload() {
    furi_hal_bt_extra_beacon_stop();
}

// Other payload functions...
BlePayload* ble_payload_create(uint8_t* data, size_t length) {
    if(!data || length == 0) return NULL;
    
    BlePayload* payload = malloc(sizeof(BlePayload));
    if(!payload) return NULL;
    
    payload->data = malloc(length);
    if(!payload->data) {
        free(payload);
        return NULL;
    }
    
    memcpy(payload->data, data, length);
    payload->length = length;
    
    return payload;
}

void ble_payload_destroy(BlePayload* payload) {
    if(!payload) return;
    
    if(payload->data) {
        free(payload->data);
        payload->data = NULL;
    }
    free(payload);
}

void ble_payload_init(BlePayload* payload, uint8_t* data, size_t length) {
    if(!payload || !data || length == 0) return;
    
    // Free existing data if any
    if(payload->data) {
        free(payload->data);
    }
    
    payload->data = malloc(length);
    if(payload->data) {
        memcpy(payload->data, data, length);
        payload->length = length;
    } else {
        payload->length = 0;
    }
}

void ble_payload_free(BlePayload* payload) {
    if(!payload) return;
    
    if(payload->data) {
        free(payload->data);
        payload->data = NULL;
    }
    payload->length = 0;
}

void ble_payload_set_data(BlePayload* payload, uint8_t* data, size_t length) {
    if(!payload || !data || length == 0) return;
    
    // Free existing data
    if(payload->data) {
        free(payload->data);
    }
    
    // Allocate new data
    payload->data = malloc(length);
    if(payload->data) {
        memcpy(payload->data, data, length);
        payload->length = length;
    } else {
        payload->length = 0;
    }
}