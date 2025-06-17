#pragma once

#include <stdbool.h>

typedef enum {
    BleSpamSceneStart,
    BleSpamSceneRunning,
    BleSpamSceneStopped,
} BleSpamSceneState;

typedef struct {
    BleSpamSceneState state;
    bool is_active;
} BleSpamScene;

void ble_spam_scene_init(BleSpamScene* scene);
void ble_spam_scene_start(BleSpamScene* scene);
void ble_spam_scene_stop(BleSpamScene* scene);
bool ble_spam_scene_is_active(BleSpamScene* scene);