#include "opengl_window.h"

void OpenGLWindow::SetOpenGLWindow(GLFWwindow *w) {
	window_ = w;
}

GLFWwindow* OpenGLWindow::GetWindow() {
	return window_;
}
