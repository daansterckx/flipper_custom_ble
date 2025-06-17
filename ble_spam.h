#pragma once

#include <furi.h>
#include "helpers/ble_payload.h"

#define MAX_PAYLOADS 10

typedef struct {
    bool is_running;
    uint32_t delay_ms;
    BlePayload payloads[MAX_PAYLOADS];
    uint32_t payload_count;
    uint32_t current_payload_index;
} BleSpam;

void ble_spam_init(void);
void ble_spam_deinit(void);
void ble_spam_worker(void* context);