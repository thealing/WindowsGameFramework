#include "window.h"

#include "platform.h"

static WNDCLASS s_window_class;

static HWND s_window;

static HDC s_device_context;

static HGLRC s_gl_context;

static bool s_mouse_button_down;

static bool s_mouse_button_pressed;

static bool s_mouse_button_released;

static bool s_keys_down[128];

static bool s_keys_pressed[128];

static bool s_keys_released[128];

static LRESULT CALLBACK window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
		case WM_SIZE:
		{
			glViewport(0, 0, LOWORD(lparam), HIWORD(lparam));

			break;
		}
		case WM_CLOSE:
		{
			DestroyWindow(window);

			break;
		}
		case WM_LBUTTONDOWN:
		{
			s_mouse_button_down = true;

			s_mouse_button_pressed = true;

			break;
		}
		case WM_LBUTTONUP:
		{
			s_mouse_button_down = false;

			s_mouse_button_released = true;

			break;
		}
		case WM_KEYDOWN:
		{
			s_keys_down[wparam] = true;

			s_keys_pressed[wparam] = true;

			break;
		}
		case WM_KEYUP:
		{
			s_keys_down[wparam] = false;

			s_keys_released[wparam] = true;

			break;
		}
		case WM_ACTIVATE:
		{
			if (LOWORD(wparam) == WA_INACTIVE)
			{
				s_mouse_button_down = false;

				memset(s_keys_down, 0, sizeof(s_keys_down));
			}

			break;
		}
	}

	return DefWindowProc(window, message, wparam, lparam);
}

void window_create(int width, int height, const char* title)
{
	s_window_class.style = CS_OWNDC;

	s_window_class.lpfnWndProc = window_proc;

	s_window_class.lpszClassName = "drmlkmqjhrkcclzgtjnc";

	s_window_class.hCursor = LoadCursor(NULL, IDC_ARROW);

	s_window_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClass(&s_window_class);

	RECT rect = { 0, 0, width, height };

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW | WS_VISIBLE, FALSE);

	s_window = CreateWindow(s_window_class.lpszClassName, title, WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, NULL, NULL);

	s_device_context = GetDC(s_window);

	PIXELFORMATDESCRIPTOR pfd = { 0 };

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);

	pfd.nVersion = 1;

	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_SWAP_COPY;

	pfd.iPixelType = PFD_TYPE_RGBA;

	pfd.cColorBits = 24;

	pfd.cDepthBits = 32;

	pfd.iLayerType = PFD_MAIN_PLANE;

	SetPixelFormat(s_device_context, ChoosePixelFormat(s_device_context, &pfd), &pfd);

	s_gl_context = wglCreateContext(s_device_context);

	wglMakeCurrent(s_device_context, s_gl_context);

	glEnable(GL_BLEND);

	glEnable(GL_TEXTURE_2D);

	glShadeModel(GL_FLAT);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void window_destroy()
{
	wglMakeCurrent(NULL, NULL);

	wglDeleteContext(s_gl_context);

	ReleaseDC(s_window, s_device_context);

	DestroyWindow(s_window);

	UnregisterClass(s_window_class.lpszClassName, NULL);
}

void window_update()
{
	s_mouse_button_pressed = false;
	
	s_mouse_button_released = false;
	
	memset(s_keys_pressed, 0, sizeof(s_keys_pressed));
	
	memset(s_keys_released, 0, sizeof(s_keys_released));
	
	MSG msg;

	while (PeekMessage(&msg, s_window, 0, 0, PM_REMOVE) > 0)
	{
		TranslateMessage(&msg);

		DispatchMessage(&msg);
	}
}

void window_display()
{
	SwapBuffers(s_device_context);
}

bool window_is_open()
{
	return IsWindow(s_window);
}

bool window_is_active()
{
	return GetActiveWindow() == s_window;
}

bool window_is_mouse_button_down()
{
	return s_mouse_button_down;
}

bool window_is_mouse_button_pressed()
{
	return s_mouse_button_pressed;
}

bool window_is_mouse_button_released()
{
	return s_mouse_button_released;
}

bool window_is_key_down(char key)
{
	return s_keys_down[key];
}

bool window_is_key_pressed(char key)
{
	return s_keys_pressed[key];
}

bool window_is_key_released(char key)
{
	return s_keys_released[key];
}

void window_get_mouse_position(double* x, double* y)
{
	POINT mouse_pos;

	GetCursorPos(&mouse_pos);

	ScreenToClient(s_window, &mouse_pos);

	double modelview_matrix[16];

	double projection_matrix[16];

	int viewport[4];

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview_matrix);

	glGetDoublev(GL_PROJECTION_MATRIX, projection_matrix);

	glGetIntegerv(GL_VIEWPORT, viewport);

	double z;

	gluUnProject(mouse_pos.x, viewport[3] - mouse_pos.y, 0.0, modelview_matrix, projection_matrix, viewport, x, y, &z);
}
