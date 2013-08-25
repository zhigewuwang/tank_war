#include "zgl.h"
#include "zfont.h"

struct Font *font;
int size = 0;
void start(Window *window) {
	window->left = window->top = 100;
	window->width = 800;
	window->height = 600;
	window->checkMultisample = false;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);
	printf("max texture size : %d\n", size);
}
int init(Window *window) {
	if (cFont("c:/Windows/Fonts/msyh.ttf", 100, &font)) return 1;
	const GLubyte* version = glGetString(GL_VERSION); //返回负责当前OpenGL实现厂商的名字
	printf("version : %s\n", version);
	glShadeModel(GL_SMOOTH);
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	return 0;
}
void resize(Window *window) {
	float halfWidth = window->width / (float)2; 
	float halfHeight = window->height / (float)2; 
	if (window->width == 0 || window->height == 0) return;
	glViewport(0, 0, window->width, window->height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(-halfWidth, halfWidth, -halfHeight, halfHeight, 0, -50);
	gluPerspective(45.0, (float)window->width / window->height, 0, 850);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void epilog(Window *window) {
	dFont(font);
}
void frameEntry(Window *window, dword ms) {
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	//glRotatef(20.0f, 0.0, 0.0, 1.0);
	glBegin(GL_TRIANGLES);
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(-100.0, -100.0, -800);
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 100.0, -800);
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(100.0, -100.0, -800);
	glEnd();
	glColor3f(0.0, .0, .0);
	glEnable(GL_TEXTURE_2D);
	font->drawText(font, L"中", 0, 0);

	glDisable(GL_TEXTURE_2D);
	glFlush();
}
