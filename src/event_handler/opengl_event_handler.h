#ifndef OPENGL_EVENT_HANDLER_H
#define OPENGL_EVENT_HANDLER_H

#include "event_handler.h"

class OpenGLEventHandler: public EventHandler {
public:
	void InitializeEventHandler(Window *w);
	void InitializeCursor(Window* w);
	
private:
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
};

#endif
