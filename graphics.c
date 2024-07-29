#include "graphics.h"

#include "platform.h"

static Texture* s_texture;

static Texture* s_font;

Image* image_load(const char* path)
{
	wchar_t wide_path[MAX_PATH];

	mbstowcs(wide_path, path, MAX_PATH);

	CoInitialize(NULL);

	IWICImagingFactory* factory;

	CoCreateInstance(&CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, &IID_IWICImagingFactory, &factory);

	IWICBitmapDecoder* decoder;

	factory->lpVtbl->CreateDecoderFromFilename(factory, wide_path, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);

	IWICBitmapFrameDecode* frame;

	decoder->lpVtbl->GetFrame(decoder, 0, &frame);

	IWICBitmapSource* bitmap;

	WICConvertBitmapSource(&GUID_WICPixelFormat32bppRGBA, (IWICBitmapSource*)frame, &bitmap);

	int width;

	int height;

	bitmap->lpVtbl->GetSize(bitmap, &width, &height);

	int row_size = width * 4;

	char* pixels = malloc(row_size * height);

	bitmap->lpVtbl->CopyPixels(bitmap, NULL, row_size, row_size * height, pixels);

	char* temp = malloc(row_size);

	for (int i = 0; i * 2 < height; i++)
	{
		char* top_row = pixels + i * row_size;

		char* bottom_row = pixels + (height - 1 - i) * row_size;

		memcpy(temp, top_row, row_size);

		memcpy(top_row, bottom_row, row_size);

		memcpy(bottom_row, temp, row_size);
	}

	free(temp);

	Image* image = malloc(sizeof(Image));

	image->width = width;

	image->height = height;

	image->pixels = pixels;

	bitmap->lpVtbl->Release(bitmap);

	frame->lpVtbl->Release(frame);

	decoder->lpVtbl->Release(decoder);

	factory->lpVtbl->Release(factory);

	CoUninitialize();

	return image;
}

void image_destroy(Image* image)
{
	free(image->pixels);

	free(image);
}

Texture* texture_create_from_file(const char* path)
{
	Image* image = image_load(path);

	Texture* texture = texture_create_from_image(image);

	image_destroy(image);

	return texture;
}

Texture* texture_create_from_image(const Image* image)
{
	Texture* texture = malloc(sizeof(Texture));

	texture->size = vector_create(image->width, image->height);

	texture->center = vector_create(image->width / 2.0, image->height / 2.0);

	texture->scale = vector_create(1.0, 1.0);

	glGenTextures(1, &texture->id);

	glBindTexture(GL_TEXTURE_2D, texture->id);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}

void texture_destroy(Texture* texture)
{
	glDeleteTextures(1, &texture->id);

	free(texture);
}

void texture_resize(Texture* texture, Vector size)
{
	texture->center = vector_scale(texture->center, vector_downscale(size, texture->size));

	texture->size = size;
}

void graphics_clear(const Color* color)
{
	glClearColor(color->r, color->g, color->b, color->a);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void graphics_set_camera(const Rect* rect)
{
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	glOrtho(rect->min.x, rect->max.x, rect->min.y, rect->max.y, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
}

void graphics_set_color(const Color* color)
{
	glColor4dv(color);
}

void graphics_set_texture(const Texture* texture)
{
	if (s_texture == texture)
	{
		return;
	}

	s_texture = texture;

	if (texture != NULL)
	{
		glBindTexture(GL_TEXTURE_2D, texture->id);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void graphics_set_font(const Texture* font)
{
	s_font = font;
}

void graphics_draw_segment(const Segment* segment, bool fill)
{
	glBegin(fill ? GL_POLYGON : GL_LINES);

	glVertex2dv(&segment->a);

	glVertex2dv(&segment->b);

	glEnd();
}

void graphics_draw_circle(const Circle* circle, bool fill)
{
	glBegin(fill ? GL_POLYGON : GL_LINE_LOOP);

	for (double angle = 0.0; angle <= M_PI * 2.0; angle += M_PI / 12.0)
	{
		glVertex2d(circle->center.x + circle->radius * cos(angle), circle->center.y + circle->radius * sin(angle));
	}

	glEnd();
}

void graphics_draw_polygon(const Polygon* polygon, bool fill)
{
	glBegin(fill ? GL_POLYGON : GL_LINE_LOOP);

	for (int i = 0; i < polygon->point_count; i++)
	{
		glVertex2dv(&polygon->points[i]);
	}

	glEnd();
}

void graphics_draw_shape(const Shape* shape, bool fill)
{
	switch (shape->type)
	{
		case SHAPE_TYPE_SEGMENT:
		{
			graphics_draw_segment(&shape->segment, fill);

			break;
		}
		case SHAPE_TYPE_CIRCLE:
		{
			graphics_draw_circle(&shape->circle, fill);

			break;
		}
		case SHAPE_TYPE_POLYGON:
		{
			graphics_draw_polygon(&shape->polygon, fill);

			break;
		}
		default:
		{
			abort();
		}
	}
}

void graphics_draw_rect(const Rect* rect, bool fill)
{
	glBegin(fill ? GL_QUADS : GL_LINE_LOOP);

	glVertex2d(rect->min.x, rect->min.y);

	glVertex2d(rect->max.x, rect->min.y);

	glVertex2d(rect->max.x, rect->max.y);

	glVertex2d(rect->min.x, rect->max.y);

	glEnd();
}

void graphics_draw_texture()
{
	if (s_texture == NULL)
	{
		return;
	}
	
	Vector scaled_center = vector_scale(s_texture->center, s_texture->scale);
	
	Vector scaled_size = vector_scale(s_texture->size, s_texture->scale);

	glBegin(GL_QUADS);

	glTexCoord2d(0.0, 0.0);

	glVertex2d(-scaled_center.x, -scaled_center.y);

	glTexCoord2d(1.0, 0.0);

	glVertex2d(-scaled_center.x + scaled_size.x, -scaled_center.y);

	glTexCoord2d(1.0, 1.0);

	glVertex2d(-scaled_center.x + scaled_size.x, -scaled_center.y + scaled_size.y);

	glTexCoord2d(0.0, 1.0);

	glVertex2d(-scaled_center.x, -scaled_center.y + scaled_size.y);

	glEnd();
}

void graphics_draw_texture_at(Vector position, double angle)
{
	if (s_texture == NULL)
	{
		return;
	}

	graphics_store();

	graphics_translate(position);

	graphics_rotate(angle);

	graphics_draw_texture();

	graphics_restore();
}

void graphics_draw_texture_in_rect(const Rect* rect)
{
	if (s_texture == NULL)
	{
		return;
	}

	glBegin(GL_QUADS);

	glTexCoord2d(0.0, 0.0);

	glVertex2d(rect->min.x, rect->min.y);

	glTexCoord2d(1.0, 0.0);

	glVertex2d(rect->max.x, rect->min.y);

	glTexCoord2d(1.0, 1.0);

	glVertex2d(rect->max.x, rect->max.y);

	glTexCoord2d(0.0, 1.0);

	glVertex2d(rect->min.x, rect->max.y);

	glEnd();
}

void graphics_draw_texture_in_quad(const Vector quad[4])
{
	if (s_texture == NULL)
	{
		return;
	}

	glBegin(GL_QUADS);

	glTexCoord2d(0.0, 0.0);

	glVertex2dv(&quad[0]);

	glTexCoord2d(1.0, 0.0);

	glVertex2dv(&quad[1]);

	glTexCoord2d(1.0, 1.0);

	glVertex2dv(&quad[2]);

	glTexCoord2d(0.0, 1.0);

	glVertex2dv(&quad[3]);

	glEnd();
}

void graphics_draw_texture_rect_in_rect(const Rect* texture_rect, const Rect* rect)
{
	if (s_texture == NULL)
	{
		return;
	}

	glBegin(GL_QUADS);

	glTexCoord2d(texture_rect->min.x, texture_rect->min.y);

	glVertex2d(rect->min.x, rect->min.y);

	glTexCoord2d(texture_rect->max.x, texture_rect->min.y);

	glVertex2d(rect->max.x, rect->min.y);

	glTexCoord2d(texture_rect->max.x, texture_rect->max.y);

	glVertex2d(rect->max.x, rect->max.y);

	glTexCoord2d(texture_rect->min.x, texture_rect->max.y);

	glVertex2d(rect->min.x, rect->max.y);

	glEnd();
}

void graphics_draw_texture_rect_in_quad(const Rect* texture_rect, const Vector quad[4])
{
	if (s_texture == NULL)
	{
		return;
	}

	glBegin(GL_QUADS);

	glTexCoord2d(texture_rect->min.x, texture_rect->min.y);

	glVertex2dv(&quad[0]);

	glTexCoord2d(texture_rect->max.x, texture_rect->min.y);

	glVertex2dv(&quad[1]);

	glTexCoord2d(texture_rect->max.x, texture_rect->max.y);

	glVertex2dv(&quad[2]);

	glTexCoord2d(texture_rect->min.x, texture_rect->max.y);

	glVertex2dv(&quad[3]);

	glEnd();
}

void graphics_draw_texture_quad_in_quad(const Vector texture_quad[4], const Vector quad[4])
{
	if (s_texture == NULL)
	{
		return;
	}

	glBegin(GL_QUADS);

	glTexCoord2dv(&texture_quad[0]);

	glVertex2dv(&quad[0]);

	glTexCoord2dv(&texture_quad[1]);

	glVertex2dv(&quad[1]);

	glTexCoord2dv(&texture_quad[2]);

	glVertex2dv(&quad[2]);

	glTexCoord2dv(&texture_quad[3]);

	glVertex2dv(&quad[3]);

	glEnd();
}

void graphics_draw_character(const Rect* rect, char character)
{
	graphics_set_texture(s_font);

	graphics_draw_texture_rect_in_rect(&(Rect){ character / 128.0, 0.0, (character + 1) / 128.0, 1.0 }, rect);
}

void graphics_draw_string_in_rect(const Rect* rect, Alignment alignment, const char* string)
{
	int string_length = strlen(string);

	double character_width = (rect->max.y - rect->min.y) / s_font->size.y * (s_font->size.x / 128.0);

	switch (alignment)
	{
		case ALIGNMENT_LEFT:
		{
			for (int i = 0; i < string_length; i++)
			{
				graphics_draw_character(&(Rect){ { rect->min.x + character_width * i, rect->min.y }, { rect->min.x + character_width * (i + 1), rect->max.y } }, string[i]);
			}

			break;
		}
		case ALIGNMENT_RIGHT:
		{
			for (int i = 0; i < string_length; i++)
			{
				graphics_draw_character(&(Rect){ { rect->max.x - character_width * (string_length - i), rect->min.y }, { rect->max.x - character_width * (string_length - i - 1), rect->max.y } }, string[i]);
			}

			break;
		}
		case ALIGNMENT_CENTER:
		{
			double start_x = (rect->min.x + rect->max.x) / 2.0 - character_width * string_length / 2.0;

			for (int i = 0; i < string_length; i++)
			{
				graphics_draw_character(&(Rect){ { start_x + character_width * i, rect->min.y }, { start_x + character_width * (i + 1), rect->max.y } }, string[i]);
			}

			break;
		}
	}
}

void graphics_draw_format_in_rect(const Rect* rect, Alignment alignment, const char* format, ...)
{
	char string[120];

	va_list args;

	va_start(args, format);

	vsprintf(string, format, args);

	va_end(args);

	graphics_draw_string_in_rect(rect, alignment, string);
}

void graphics_draw_string(Alignment horizontal_alignment, Alignment vertical_alignment, const char* string)
{
	int string_length = strlen(string);

	double character_width = (s_font->size.x / 128.0) / s_font->size.y;

	double start_x = 0.0;

	double lower_y = 0.0;

	double upper_y = 0.0;

	switch (horizontal_alignment)
	{
		case ALIGNMENT_LEFT:
		{
			start_x = 0.0;

			break;
		}
		case ALIGNMENT_RIGHT:
		{
			start_x = character_width * -string_length;

			break;
		}
		case ALIGNMENT_CENTER:
		{
			start_x = character_width * -string_length / 2.0;

			break;
		}
	}

	switch (vertical_alignment)
	{
		case ALIGNMENT_BOTTOM:
		{
			lower_y = 0.0;

			upper_y = 1.0;

			break;
		}
		case ALIGNMENT_TOP:
		{
			lower_y = -1.0;

			upper_y = 0.0;

			break;
		}
		case ALIGNMENT_CENTER:
		{
			lower_y = -0.5;

			upper_y = 0.5;

			break;
		}
	}

	for (int i = 0; i < string_length; i++)
	{
		graphics_draw_character(&(Rect){ { start_x + character_width * i, lower_y }, { start_x + character_width * (i + 1), upper_y } }, string[i]);
	}
}

void graphics_draw_format(Alignment horizontal_alignment, Alignment vertical_alignment, const char* format, ...)
{
	char string[120];

	va_list args;

	va_start(args, format);

	vsprintf(string, format, args);

	va_end(args);

	graphics_draw_string(horizontal_alignment, vertical_alignment, string);
}

void graphics_store()
{
	glPushMatrix();
}

void graphics_restore()
{
	glPopMatrix();
}

void graphics_translate(Vector translation)
{
	glTranslated(translation.x, translation.y, 0.0);
}

void graphics_rotate(double angle)
{
	glRotated(angle * 180.0 / M_PI, 0.0, 0.0, 1.0);
}

void graphics_scale(Vector factor)
{
	glScaled(factor.x, factor.y, 1.0);
}

void graphics_scale_uniformly(double factor)
{
	glScaled(factor, factor, 1.0);
}
