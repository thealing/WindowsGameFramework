#pragma once

#include "geometry.h"

#include <stdio.h>

typedef enum Alignment Alignment;

typedef struct Color Color;

typedef struct Image Image;

typedef struct Texture Texture;

enum Alignment
{
	ALIGNMENT_LEFT,

	ALIGNMENT_RIGHT,

	ALIGNMENT_BOTTOM,

	ALIGNMENT_TOP,

	ALIGNMENT_CENTER,

	ALIGNMENT_COUNT
};

struct Color
{
	double r;

	double g;

	double b;

	double a;
};

struct Image
{
	int width;

	int height;

	void* pixels;
};

struct Texture
{
	Vector size;

	Vector center;
	
	Vector scale;

	int id;
};

Image* image_load(const char* path);

void image_destroy(Image* image);

Texture* texture_create_from_file(const char* path);

Texture* texture_create_from_image(const Image* image);

void texture_destroy(Texture* texture);

void texture_resize(Texture* texture, Vector size);

void graphics_clear(const Color* color);

void graphics_set_camera(const Rect* rect);

void graphics_set_color(const Color* color);

void graphics_set_texture(const Texture* texture);

void graphics_set_font(const Texture* font);

void graphics_draw_segment(const Segment* segment, bool fill);

void graphics_draw_circle(const Circle* circle, bool fill);

void graphics_draw_polygon(const Polygon* polygon, bool fill);

void graphics_draw_shape(const Shape* shape, bool fill);

void graphics_draw_rect(const Rect* rect, bool fill);

void graphics_draw_texture();

void graphics_draw_texture_at(Vector position, double angle);

void graphics_draw_texture_in_rect(const Rect* rect);

void graphics_draw_texture_in_quad(const Vector quad[4]);

void graphics_draw_texture_rect_in_rect(const Rect* texture_rect, const Rect* rect);

void graphics_draw_texture_rect_in_quad(const Rect* texture_rect, const Vector quad[4]);

void graphics_draw_texture_quad_in_quad(const Vector texture_quad[4], const Vector quad[4]);

void graphics_draw_character(const Rect* rect, char character);

void graphics_draw_string_in_rect(const Rect* rect, Alignment alignment, const char* string);

void graphics_draw_format_in_rect(const Rect* rect, Alignment alignment, const char* format, ...);

void graphics_draw_string(Alignment horizontal_alignment, Alignment vertical_alignment, const char* string);

void graphics_draw_format(Alignment horizontal_alignment, Alignment vertical_alignment, const char* format, ...);

void graphics_store();

void graphics_restore();

void graphics_translate(Vector translation);

void graphics_rotate(double angle);

void graphics_scale(Vector factor);

void graphics_scale_uniformly(double factor);
