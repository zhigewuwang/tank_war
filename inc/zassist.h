#ifndef ZMSGBOX_H
#define ZMSGBOX_H

#include <stdio.h>


int wglIsExtensionSupported(const char *extension);

void msgbox(unsigned int type, wchar_t *title, wchar_t *format, ...);
void alert(wchar_t *title, wchar_t *format, ...);

/**
 * @return int 1 代表选择否, 0 代表选择 是
 */
int confirm(wchar_t *title, wchar_t *format, ...);

/**
 * 是否显示鼠标,因为 WIN32 API ShowCursor 维护内部计数器
 * 当ShowCursor(FALSE), 计数器-1，否则+1
 * 当计数器>=0时，显示鼠标 
 * 此函数简化该API调用, visiable 为真时显示，为假时隐藏
 */
void showCursor(int visiable);

#endif

