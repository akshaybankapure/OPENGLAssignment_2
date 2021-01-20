#ifndef OEPNGL_WINDOW_H
#define OEPNGL_WINDOW_H

#include "window.h"

class OpenGLWindow : public Window {
public:
	GLFWwindow *window_;
	void SetOpenGLWindow(GLFWwindow *w);
	GLFWwindow* GetWindow();
};

#endif
