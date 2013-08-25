#include <stdio.h>
#include "zgl.h"

int size = 0;

void keydown(struct Window *window) {
	if (window->key_status[VK_ESCAPE]) {
		window->close(window);
	} else if (window->key_status[VK_F1]) {
		window->change_mode(window, WINDOW_MODE_WINDOW);
	} else if (window->key_status[VK_F2]) {
		window->change_mode(window, WINDOW_MODE_FULL_SCREEN);
	} else if (window->key_status[VK_F3]) {
		window->change_mode(window, WINDOW_MODE_PSEUDO_FULL_SCREEN);
	}
}
void keyup(struct Window *window) {
}
void init(struct Window *window) {
	/*
	window->left = window->top = 100;
	window->width = 800;
	window->height = 600;
	window->check_multisample = 0;
	*/
	//window->window_mode = WINDOW_MODE_PSEUDO_FULL_SCREEN;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);
	printf("max texture size : %d\n", size);
}
int prolog(struct Window *window) {
	const GLubyte* version = glGetString(GL_VERSION); //返回负责当前OpenGL实现厂商的名字
	printf("version : %s\n", version);
	glShadeModel(GL_SMOOTH);
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	printf("prolog\n");
	return 0;
}
void resize(struct Window *window) {
	float halfWidth = window->width / (float)2; 
	float halfHeight = window->height / (float)2; 
	printf("resize\n");
	if (window->width == 0 || window->height == 0) return;
	glViewport(0, 0, window->width, window->height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(-halfWidth, halfWidth, -halfHeight, halfHeight, 0, -50);
	gluPerspective(45.0, (float)window->width / window->height, 0, 850);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void epilog(struct Window *window) {
	printf("epilog\n");
}
void render(struct Window *window) {
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

	glDisable(GL_TEXTURE_2D);
	glFlush();
}
