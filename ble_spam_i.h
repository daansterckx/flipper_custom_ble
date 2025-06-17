#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

typedef enum {
    SpamEventTypeInput,
} SpamEventType;

typedef struct {
    SpamEventType type;
    InputEvent input;
} SpamEvent;

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    NotificationApp* notifications;
    bool running;
    int current_spam_mode;
    char* custom_names[5];
} BLESpam;