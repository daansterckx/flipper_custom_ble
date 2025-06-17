#include "ble_spam_scene.h"
#include "../helpers/ble_payload.h"
#include <furi.h>

void ble_spam_scene_init(BleSpamScene* scene) {
    if(!scene) return;
    
    scene->state = BleSpamSceneStart;
    scene->is_active = false;
}

void ble_spam_scene_start(BleSpamScene* scene) {
    if(!scene) return;
    
    scene->state = BleSpamSceneRunning;
    scene->is_active = true;
}

void ble_spam_scene_stop(BleSpamScene* scene) {
    if(!scene) return;
    
    scene->state = BleSpamSceneStopped;
    scene->is_active = false;
    
    // Stop any active BLE transmission
    ble_stop_payload();
}

bool ble_spam_scene_is_active(BleSpamScene* scene) {
    return scene ? scene->is_active : false;
}