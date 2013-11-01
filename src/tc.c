#include <stdio.h>
#include "zgl.h"

int size = 0;
GLuint bufId = 0;

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
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);
	printf("max texture size : %d\n", size);

	GLenum err = glewInit();
	if (GLEW_OK != err)  {  
		printf("Error: %s\n", glewGetErrorString(err));  
	}  
	
	printf("glIsBuffer \n");
	glGenBuffers(1, &bufId);
	printf("glIsBuffer end \n");
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
	// left, right, bottom, top, near, far
	glOrtho(0.0f, (float)window->width, 0.0f, (float)window->height, 0, 100);
	//glOrtho(-halfWidth, halfWidth, -halfHeight, halfHeight, 0, 100);
	//gluPerspective(45.0, (float)window->width / window->height, 0, 850);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void epilog(struct Window *window) {
	printf("epilog\n");
}
void render(struct Window *window) {
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	glFlush();
}
