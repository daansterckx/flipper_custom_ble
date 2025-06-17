#include "ble_spam_app.h"
#include "ble_spam.h"
#include "helpers/ble_payload.h"
#include <furi_hal_bt.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include <furi_hal.h>
#include <furi.h>

// REMOVE all problematic BT service includes
#include <notification/notification_messages.h>

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    BleSpam* ble_spam;
    bool is_spamming;
    uint32_t current_attack;
    const char** attack_names;
    uint32_t attack_count;
    FuriThread* worker_thread;
    bool bt_connected;
} BleSpamApp;

static int32_t ble_spam_worker_thread(void* context) {
    BleSpam* ble_spam = context;
    if(!ble_spam) return -1;
    
    while(ble_spam->is_running) {
        if(ble_spam->payload_count > 0) {
            // Cycle through payloads
            uint32_t index = ble_spam->current_payload_index % ble_spam->payload_count;
            BlePayload* current_payload = &ble_spam->payloads[index];
            if(current_payload && current_payload->data && current_payload->length > 0) {
                // Try to send payload
                bool success = ble_send_payload(current_payload);
                
                if(!success) {
                    // Add small delay on failure
                    furi_delay_ms(10);
                }
            }
            
            ble_spam->current_payload_index = (ble_spam->current_payload_index + 1) % ble_spam->payload_count;
        }
        
        furi_delay_ms(ble_spam->delay_ms);
    }
    
    return 0;
}

static void ble_spam_app_draw_callback(Canvas* canvas, void* context) {
    BleSpamApp* app = context;
    if(!app) return;
    
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 15, "BLE Spam");
    
    canvas_set_font(canvas, FontSecondary);
    if(app->is_spamming) {
        canvas_draw_str(canvas, 2, 30, "Status: Running");
        canvas_draw_str(canvas, 2, 45, app->attack_names[app->current_attack]);
        canvas_draw_str(canvas, 2, 60, "Press OK to stop");
    } else {
        canvas_draw_str(canvas, 2, 30, "Status: Stopped");
        canvas_draw_str(canvas, 2, 45, app->attack_names[app->current_attack]);
        canvas_draw_str(canvas, 2, 60, "Press OK to start");
        canvas_draw_str(canvas, 2, 75, "Press Up/Down to select");
    }
}

static void ble_spam_app_input_callback(InputEvent* input_event, void* context) {
    BleSpamApp* app = context;
    if(!app || !input_event) return;
    
    furi_message_queue_put(app->event_queue, input_event, FuriWaitForever);
}

static bool connect_to_bt(BleSpamApp* app) {
    if(!app) return false;
    
    // SIMPLIFIED: Just start BT advertising directly 
    // without trying to use bt_reserve
    
    // Initialize BLE stack if needed
    if(!furi_hal_bt_is_alive()) {
        furi_hal_bt_init();
        furi_delay_ms(200);
    }
    
    furi_hal_bt_start_advertising();
    app->bt_connected = true;
    
    return true;
}

static void disconnect_from_bt(BleSpamApp* app) {
    if(!app) return;
    
    // Stop any active beacons first
    furi_hal_bt_extra_beacon_stop();
    furi_delay_ms(10);
    
    // Make sure advertising is stopped
    furi_hal_bt_stop_advertising();
    
    app->bt_connected = false;
}

static bool start_spam(BleSpamApp* app) {
    if(!app) return false;
    
    // Check if BLE is already running
    if(app->is_spamming) return false;
    
    // Make sure we have a connection to BT
    if(!app->bt_connected) {
        if(!connect_to_bt(app)) {
            FURI_LOG_E("BLE_SPAM", "Failed to connect to BT");
            return false;
        }
    }
    
    // Create initial payload based on the selected attack
    BlePayload* payload = &app->ble_spam->payloads[0];
    
    // Make sure payload is clean before creating
    if(payload->data) {
        free(payload->data);
        payload->data = NULL;
        payload->length = 0;
    }
    
    // Create payload
    create_ble_payload(payload, app->attack_names[app->current_attack]);
    
    // Check if payload creation was successful
    if(!payload->data || payload->length == 0) {
        FURI_LOG_E("BLE_SPAM", "Failed to create payload");
        return false;
    }
    
    app->ble_spam->payload_count = 1;
    
    // Start worker thread
    app->ble_spam->is_running = true;
    
    app->worker_thread = furi_thread_alloc();
    furi_thread_set_name(app->worker_thread, "BleSpamWorker");
    furi_thread_set_stack_size(app->worker_thread, 1024);
    furi_thread_set_context(app->worker_thread, app->ble_spam);
    furi_thread_set_callback(app->worker_thread, ble_spam_worker_thread);
    
    // Start thread
    furi_thread_start(app->worker_thread);
    
    app->is_spamming = true;
    return true;
}

// Rest of your code remains unchanged

static void stop_spam(BleSpamApp* app) {
    if(!app || !app->ble_spam) return;
    
    // Stop worker thread
    if(app->worker_thread) {
        // Signal thread to stop
        app->ble_spam->is_running = false;
        
        // Give thread a moment to exit with a small delay first
        furi_delay_ms(100);
        
        // Join thread - this will wait for the thread to finish
        furi_thread_join(app->worker_thread);
        furi_thread_free(app->worker_thread);
        app->worker_thread = NULL;
    }
    
    // Free payload data with safety checks
    for(uint32_t i = 0; i < app->ble_spam->payload_count; i++) {
        if(app->ble_spam->payloads[i].data) {
            free(app->ble_spam->payloads[i].data);
            app->ble_spam->payloads[i].data = NULL;
        }
        app->ble_spam->payloads[i].length = 0;
    }
    app->ble_spam->payload_count = 0;
    
    app->is_spamming = false;
}

BleSpamApp* ble_spam_app_alloc() {
    BleSpamApp* app = malloc(sizeof(BleSpamApp));
    if(!app) return NULL;
    
    // Initialize to zero first
    memset(app, 0, sizeof(BleSpamApp));
    
    // Initialize attack names
    static const char* attacks[] = {
        "iOS Popup Spam",
        "Samsung Buds",
        "AirPods",
        "Generic BLE",
    };
    
    app->attack_names = attacks;
    app->attack_count = 4;
    app->current_attack = 0;
    app->is_spamming = false;
    app->worker_thread = NULL;
    app->bt_connected = false;
    
    // Initialize BLE spam
    app->ble_spam = malloc(sizeof(BleSpam));
    if(!app->ble_spam) {
        free(app);
        return NULL;
    }
    
    memset(app->ble_spam, 0, sizeof(BleSpam));
    app->ble_spam->delay_ms = 100;
    
    // Initialize GUI
    app->gui = furi_record_open(RECORD_GUI);
    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    
    view_port_draw_callback_set(app->view_port, ble_spam_app_draw_callback, app);
    view_port_input_callback_set(app->view_port, ble_spam_app_input_callback, app);
    
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);
    
    return app;
}

void ble_spam_app_free(BleSpamApp* app) {
    if(!app) return;
    
    // Make sure spam is stopped
    stop_spam(app);
    
    // Make sure we release BT resources
    disconnect_from_bt(app);
    
    // Free UI resources
    if(app->gui && app->view_port) {
        gui_remove_view_port(app->gui, app->view_port);
    }
    
    if(app->view_port) {
        view_port_free(app->view_port);
    }
    
    if(app->event_queue) {
        furi_message_queue_free(app->event_queue);
    }
    
    if(app->gui) {
        furi_record_close(RECORD_GUI);
    }
    
    // Free app resources
    if(app->ble_spam) {
        free(app->ble_spam);
    }
    
    free(app);
}

int32_t ble_spam_app_main(void* p) {
    UNUSED(p);
    
    // Allocate application
    BleSpamApp* app = ble_spam_app_alloc();
    if(!app) {
        return 255;
    }
    
    // Initialize BT connection
    if(!connect_to_bt(app)) {
        FURI_LOG_E("BLE_SPAM", "Failed to connect to BT, continuing with limited functionality");
        // Continue anyway, we'll try again when starting spam
    }
    
    InputEvent event;
    bool running = true;
    
    while(running) {
        // Add timeout to message queue get to prevent hanging
        FuriStatus status = furi_message_queue_get(app->event_queue, &event, 100);
        
        if(status == FuriStatusOk) {
            if(event.type == InputTypePress) {
                switch(event.key) {
                    case InputKeyOk:
                        if(app->is_spamming) {
                            stop_spam(app);
                        } else {
                            if(!start_spam(app)) {
                                // If start fails, don't crash - just continue
                                FURI_LOG_E("BLE_SPAM", "Failed to start spam");
                            }
                        }
                        break;
                    case InputKeyUp:
                        if(!app->is_spamming && app->attack_count > 0) {
                            app->current_attack = (app->current_attack + app->attack_count - 1) % app->attack_count;
                        }
                        break;
                    case InputKeyDown:
                        if(!app->is_spamming && app->attack_count > 0) {
                            app->current_attack = (app->current_attack + 1) % app->attack_count;
                        }
                        break;
                    case InputKeyBack:
                        running = false;
                        break;
                    default:
                        break;
                }
            }
        }
        
        // Update UI
        view_port_update(app->view_port);
    }
    
    // Free application
    ble_spam_app_free(app);
    return 0;
}