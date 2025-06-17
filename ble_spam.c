#include "ble_spam.h"
#include "helpers/ble_payload.h"
#include <furi_hal_bt.h>
#include <furi.h>

void ble_spam_init() {
    // Start BLE advertising
    furi_hal_bt_start_advertising();
}

void ble_spam_deinit() {
    // Stop beacon and advertising
    furi_hal_bt_extra_beacon_stop();
    furi_hal_bt_stop_advertising();
}

void ble_spam_worker(void* context) {
    BleSpam* ble_spam = context;
    if(!ble_spam) return;
    
    while(ble_spam->is_running) {
        if(ble_spam->payload_count > 0) {
            // Cycle through payloads
            BlePayload* current_payload = &ble_spam->payloads[ble_spam->current_payload_index];
            
            // Only send if payload contains valid data
            if(current_payload && current_payload->data && current_payload->length > 0) {
                ble_send_payload(current_payload);
            }
            
            ble_spam->current_payload_index = (ble_spam->current_payload_index + 1) % ble_spam->payload_count;
        }
        
        furi_delay_ms(ble_spam->delay_ms);
    }
}