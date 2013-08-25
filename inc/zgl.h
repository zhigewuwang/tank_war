#ifndef ZGL_H

#define ZGL_H
#define UNICODE

#define WINDOW_MODE_WINDOW 0
#define WINDOW_MODE_FULL_SCREEN 1
#define WINDOW_MODE_PSEUDO_FULL_SCREEN 2

#define GLEW_STATIC 
#define GLEW_BUILD 
#include <windows.h>
#include <GL/glew.h>
#include <GL/wglew.h>

struct Window {
	HINSTANCE hinstance;
	wchar_t *class_name;
	char key_status[256]; // 0 没按下， 1按下
	wchar_t *title;
	int left;
	int top;
	int width;
	int height;
	int bits_per_pixel;
	int window_mode; // window, fullscreen, pseudo fullscreen
	HWND hwnd;
	HDC hdc;
	HGLRC hrc;
	int dpi_x;
	int dpi_y;
	int is_hardware_acceleration;
	int check_multisample;
	int is_arb_multisample_supported;
	int arb_multisample_format;

	int (*change_mode)(struct Window *window, int mode);
	void (*close)(struct Window *window);
};

// 用户需要实现的函数
void init(struct Window *window); // 场下准备
int prolog(struct Window *window); //  入场准备
void keydown(struct Window *window);
void keyup(struct Window *window);
void resize(struct Window *window); 
void render(struct Window *window); //每帧入口， ms 代表前帧至令的毫秒数   绘制程序在此函数中
void epilog(struct Window *window); // 收场

#endif
