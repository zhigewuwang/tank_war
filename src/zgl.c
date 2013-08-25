#include "zgl.h"
#include "zassist.h"
#include <stdio.h>

#define WGL_SAMPLE_BUFFERS_ARB 0x2041
#define WGL_SAMPLES_ARB 0x2042

struct SizeWindow {
	struct Window window;
	int old_left;
	int old_top;
	int old_width;
	int old_height;
};

int active = 1;  // 是否进入 render 函数, 主要用来  在最小化时  不进行渲染了

static void close(struct Window *window) {
	PostQuitMessage(0);
}

LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	CREATESTRUCT *creation;
	struct Window *window = (struct Window *)GetWindowLong(hwnd, GWL_USERDATA);
	switch (msg) {
		case WM_KEYDOWN : 
			if (wp >= 0 && wp <= 255) {
				window->key_status[wp] = 1;
				keydown(window);		
			}
			return 0;
		case WM_KEYUP :
			if (wp >= 0 && wp <= 255) {
				window->key_status[wp] = 0;
				keyup(window);		
			}
			return 0;
		case WM_CREATE :
			creation = (CREATESTRUCT *)lp;
			window = (struct Window *)creation->lpCreateParams;
			SetWindowLong(hwnd, GWL_USERDATA, (LONG)window);
			return 0;
		case WM_CLOSE :
			close(window);
			return 0;
		case WM_SIZE : 
			window->width = LOWORD(lp);
			window->height = HIWORD(lp);
			printf("resize %d %d\n", window->width, window->height);
			resize(window);
			return 0;
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}

static int init_multisample(struct Window *window, PIXELFORMATDESCRIPTOR pfd) {
	int pixelFormat;
	int valid;
	UINT numFormats;
	float fattrs[] = {0, 0};
	int iattrs[] = {
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		WGL_COLOR_BITS_ARB, 24,
		WGL_ALPHA_BITS_ARB, 8,
		WGL_DEPTH_BITS_ARB, 16,
		WGL_STENCIL_BITS_ARB, 0,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
		WGL_SAMPLES_ARB, 4,
		0, 0
	};
	if (!wglIsExtensionSupported("WGL_ARB_multisample")) return 0;

	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");

	if (!wglChoosePixelFormatARB) return 0;

	valid = wglChoosePixelFormatARB(window->hdc, iattrs, fattrs, 1, &pixelFormat, &numFormats);
	if (valid && numFormats >= 1) {
		window->arb_multisample_format = pixelFormat;
		return window->is_arb_multisample_supported = 1;
	}

	iattrs[19] = 2;
	valid = wglChoosePixelFormatARB(window->hdc, iattrs, fattrs, 1, &pixelFormat, &numFormats);
	if (valid && numFormats >= 1) {
		window->arb_multisample_format = pixelFormat;
		return window->is_arb_multisample_supported = 1;
	}
	return window->is_arb_multisample_supported = 0;
}

static int change_screen_resolution(struct Window *window){
	wchar_t *msg = (wchar_t *)malloc(sizeof(wchar_t) * 1000);
	DEVMODE dm;
	memset(&dm, 0, sizeof(DEVMODE));
	dm.dmSize = sizeof(DEVMODE);
	dm.dmPelsWidth = window->width;
	dm.dmPelsHeight = window->height;
	dm.dmBitsPerPel = window->bits_per_pixel;
	dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	return ChangeDisplaySettings(&dm, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL;
}

static void create_glcontext(struct Window *window) {
	GLuint pixelFormat;
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		window->bits_per_pixel,
		0, 0, 0, 0, 0, 0,
		0, 
		0, 
		0, 
		0, 0, 0, 0, 
		16,
		0, 
		0, 
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};
	if (window->check_multisample && init_multisample(window, pfd)) {
		pixelFormat = window->arb_multisample_format;
	} else pixelFormat = ChoosePixelFormat(window->hdc, &pfd);

	SetPixelFormat(window->hdc, pixelFormat, &pfd);

	DescribePixelFormat(window->hdc, pixelFormat, sizeof(pfd), &pfd);
	window->is_hardware_acceleration = !(pfd.dwFlags & PFD_GENERIC_FORMAT) && !(pfd.dwFlags & PFD_GENERIC_ACCELERATED);

	window->hrc = wglCreateContext(window->hdc);
	if (!window->hrc)
		printf("failed\n");
	wglMakeCurrent(window->hdc, window->hrc);
}
static void locate_size(struct Window *window, RECT *rect, DWORD *exStyle, DWORD *style){
	struct SizeWindow *sw = (struct SizeWindow *)window;
	if (sw->old_left != -1) {
		window->left = sw->old_left;window->top = sw->old_top;window->width = sw->old_width; window->height = sw->old_height;
		rect->left = window->left; rect->top = window->top; rect->right = window->left + window->width;
		rect->bottom = window->top + window->height;
		sw->old_left = -1;
	}

	if (window->window_mode == WINDOW_MODE_FULL_SCREEN) {
		if (!change_screen_resolution(window)) {
			window->window_mode = WINDOW_MODE_WINDOW;
		} else {
			ShowCursor(0);
			rect->left = 0;
			rect->top = 0;
			rect->right = GetSystemMetrics(SM_CXSCREEN);
			rect->bottom = GetSystemMetrics(SM_CYSCREEN);
			*style = WS_POPUP | WS_MAXIMIZE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
			*exStyle = WS_EX_APPWINDOW | WS_EX_TOPMOST;
		}
	} else if (window->window_mode == WINDOW_MODE_PSEUDO_FULL_SCREEN) {
		*exStyle = WS_EX_APPWINDOW | WS_EX_TOPMOST;
		*style = WS_POPUP | WS_MAXIMIZE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		sw->old_left = window->left; sw->old_top = window->top; sw->old_width = window->width; sw->old_height = window->height;
		window->left = rect->left = 0;
		window->top = rect->top = 0;
		window->width = rect->right = GetSystemMetrics(SM_CXSCREEN);
		window->height = rect->bottom = GetSystemMetrics(SM_CYSCREEN);
		ShowCursor(0);
	}
	if (window->window_mode == WINDOW_MODE_WINDOW) {
		*exStyle = WS_EX_APPWINDOW;
		*style = WS_OVERLAPPEDWINDOW;
		AdjustWindowRectEx(rect, *style, 0, *exStyle);
	}
}
static void create_glwindow(struct Window *window){
	WNDCLASS wc;
	memset(&wc, 0, sizeof(wc));
	wc.lpfnWndProc = wnd_proc;
	wc.lpszClassName = window->class_name;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.hInstance = window->hinstance;

	RegisterClass(&wc);
	
	RECT rect = { window->left, window->top, window->left + window->width, window->top + window->height};
	DWORD ex_style = 0;
	DWORD style = 0;
	
	locate_size(window, &rect, &ex_style, &style);
	printf("%d %d %d %d %d %d %d %d\n", window->left, window->top, window->width, window->height,
			rect.left, rect.top, rect.right, rect.bottom);
	window->hwnd = CreateWindowEx(
			ex_style,
			window->class_name,
			window->title,
			style,
			rect.left, rect.top,
			rect.right - rect.left,
			rect.bottom - rect.top,
			0,//HWND_DESKTOP,
			0,
			window->hinstance,
			window
	);

	window->hdc = GetDC(window->hwnd);
	window->dpi_x = GetDeviceCaps(window->hdc, LOGPIXELSX);
	window->dpi_y = GetDeviceCaps(window->hdc, LOGPIXELSY);

	create_glcontext(window);

	memset(window->key_status, 0, sizeof(window->key_status));
	ShowWindow(window->hwnd, SW_NORMAL);
	UpdateWindow(window->hwnd);
	prolog(window);
}
static void destroy_glwindow(struct Window *window) {
	epilog(window);

	if (window->hrc) {
		wglDeleteContext(window->hrc);
		window->hrc = 0;
	}
	if (window->hdc) {
		ReleaseDC(window->hwnd, window->hdc);
		window->hdc = 0;
	}
	if (window->hwnd) {
		DestroyWindow(window->hwnd);
		window->hwnd = 0;
	}

	if (window->window_mode == WINDOW_MODE_FULL_SCREEN) {
		printf("full screen change display setting\n");
		ChangeDisplaySettings(0, 0);
		ShowCursor(1);
	}

	UnregisterClass(window->class_name, window->hinstance);
}

static int change_mode(struct Window *window, int mode) {
	if (mode == window->window_mode 
			|| (mode != WINDOW_MODE_WINDOW && mode != WINDOW_MODE_FULL_SCREEN 
				&& mode != WINDOW_MODE_PSEUDO_FULL_SCREEN)) return 1;
	destroy_glwindow(window);
	window->window_mode = mode;
	create_glwindow(window);
}

int WINAPI WinMain(HINSTANCE hins, HINSTANCE parent_hiins, LPSTR cmdLine, int showCmd){
	MSG msg;
	struct SizeWindow size_window;
	struct Window *window = (struct Window *)&size_window;

	memset(&size_window, 0, sizeof(struct SizeWindow));

	window->hinstance = hins;
	window->class_name = L"openGL";

	window->title = L"zgl framework";
	window->width = 640;
	window->height = 480;
	window->left = 100;
	window->top = 100;
	window->bits_per_pixel = 32;
	window->window_mode = WINDOW_MODE_WINDOW;
	// 不检查是否支持多重采样，所以也不会开启
	window->check_multisample = 0;
	window->change_mode = &change_mode;
	window->close = &close;
	
	size_window.old_left = -1;

	init(window);

	create_glwindow(window);

	while (1) {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			} else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		} else if (active) {
			render(window);
			SwapBuffers(window->hdc);
			Sleep(20);
		}
	}
	destroy_glwindow(window);
	return 0;
}
