#pragma once

#include <stdbool.h>

#define KEY_NONE 0

#define KEY_BACK 8

#define KEY_ENTER 13

#define KEY_SPACE 32

#define KEY_LEFT 37

#define KEY_UP 38

#define KEY_RIGHT 39

#define KEY_DOWN 40

void window_create(int width, int height, const char* title);

void window_destroy();

void window_update();

void window_display();

bool window_is_open();

bool window_is_active();

bool window_is_mouse_button_down();

bool window_is_mouse_button_pressed();

bool window_is_mouse_button_released();

bool window_is_key_down(char key);

bool window_is_key_pressed(char key);

bool window_is_key_released(char key);

void window_get_mouse_position(double* x, double* y);
