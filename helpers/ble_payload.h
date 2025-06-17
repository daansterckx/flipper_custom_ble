#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct {
    uint8_t* data;
    size_t length;
} BlePayload;

void create_ble_payload(BlePayload* payload, const char* custom_name);
bool ble_send_payload(BlePayload* payload);
void ble_stop_payload(void);

BlePayload* ble_payload_create(uint8_t* data, size_t length);
void ble_payload_destroy(BlePayload* payload);

void ble_payload_init(BlePayload* payload, uint8_t* data, size_t length);
void ble_payload_free(BlePayload* payload);
void ble_payload_set_data(BlePayload* payload, uint8_t* data, size_t length);