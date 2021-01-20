#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>

class Window {
public:
	virtual void SetOpenGLWindow(GLFWwindow *w) = 0;
	virtual GLFWwindow* GetWindow() = 0;
};

#endif
