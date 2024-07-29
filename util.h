#pragma once

#include "physics.h"

#include "graphics.h"

#include "window.h"

#define countof(a) (sizeof(a) / sizeof((a)[0]))

Shape* move_shape(Shape* shape);

Shape* create_rect_shape(Vector min, Vector max);

void set_texture_and_color(const Texture* texture, const Color* color);

void draw_physics_world(const Physics_World* world);

void draw_physics_body(const Physics_Body* body);

void draw_physics_collider(const Physics_Collider* collider);

void draw_physics_joint(const Physics_Joint* joint);

Vector get_mouse_position();
