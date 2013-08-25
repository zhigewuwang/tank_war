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

#endif

