#define UNICODE
#include <stdarg.h>
#include "zassist.h"
#include <windows.h>
#include <GL/glew.h>
#include <GL/wglew.h>


int wglIsExtensionSupported(const char *extension) {
	const size_t extlen = strlen(extension);
	const char *supported = 0;
	const char *p;
	PROC wglGetExtString = wglGetProcAddress("wglGetExtensionsStringARB");
	if (wglGetExtString) 
		supported = ((char *(__stdcall *)(HDC))wglGetExtString)(wglGetCurrentDC());
	if (supported == 0) supported = (char *)glGetString(GL_EXTENSIONS);
	if (supported == 0) return 0;
	for (p = supported; ; p++) {
		p = strstr(p, extension);
		if (p == 0) return 0;
		if ((p == supported || p[-1] == ' ') && (p[extlen] == '\0' || p[extlen] == ' ')) return 1;
	}
}

void msgbox(unsigned int type, wchar_t *title, wchar_t *format, ...){
	wchar_t buf[256];
	va_list args;
	va_start(args, format);
	vswprintf(buf, format, args);
	MessageBox(0, buf, title, type);
	va_end(args);
}

void alert(wchar_t *title, wchar_t *format, ...) {
	wchar_t buf[256];
	va_list args;
	va_start(args, format);
	vswprintf(buf, format, args);
	MessageBox(0, buf, title, MB_OK);
	va_end(args);
}
int confirm(wchar_t *title, wchar_t *format, ...) {
	wchar_t buf[256];
	va_list args;
	va_start(args, format);
	vswprintf(buf, format, args);
	int select = MessageBox(0, buf, title, MB_YESNO);
	va_end(args);
	return select == IDNO ? 1 : 0;
}
