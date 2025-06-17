#pragma once

#include <gui/view.h>
#include <gui/canvas.h>
#include <stdbool.h>

void ble_spam_view_draw(Canvas* canvas, void* model);
bool send_demo_name(void);
bool send_demo_name_with_text(const char* custom_text);