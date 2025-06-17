#include "ble_spam_view.h"
#include "../helpers/ble_payload.h"
#include <furi.h>
#include <gui/gui.h>
#include <stdlib.h>

#define DEMO_NAME "DemoDevice"

void ble_spam_view_draw(Canvas* canvas, void* model) {
    UNUSED(model);
    
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 15, "BLE Spam View");
    
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 30, "Broadcasting...");
}

bool send_demo_name() {
    const char* name = DEMO_NAME;
    BlePayload payload = {0};
    
    // Create BLE payload with demo name
    create_ble_payload(&payload, name);
    
    // Send the payload
    bool result = false;
    if(ble_send_payload(&payload)) {
        result = true;
    }
    
    // Clean up
    if(payload.data) {
        free(payload.data);
    }
    
    return result;
}

bool send_demo_name_with_text(const char* custom_text) {
    if(!custom_text) return false;
    
    BlePayload payload = {0};
    
    // Create BLE payload with custom text
    create_ble_payload(&payload, custom_text);
    
    // Send the payload
    bool result = false;
    if(ble_send_payload(&payload)) {
        result = true;
    }
    
    // Clean up
    if(payload.data) {
        free(payload.data);
    }
    
    return result;
}