#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#ifndef M_PI
	#define M_PI 3.142
#endif

#include "../global.h"
#include "../window/opengl_window.h"

#include "opengl_event_handler.h"

void OpenGLEventHandler::InitializeEventHandler(Window *w) {
	glfwSetFramebufferSizeCallback(w->GetWindow(), framebuffer_size_callback);
	glfwSetKeyCallback(w->GetWindow(), key_callback);

}

void OpenGLEventHandler::InitializeCursor(Window* w)
{
	glfwSetInputMode(w->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(w->GetWindow(), cursor_position_callback);
}

void OpenGLEventHandler::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// Make sure the viewport matches the new window dimations; note that width and
	// height will be significantly larger than specified on retina displays.
	scr_width = width;
	scr_height = height;
	glViewport(0, 0, width, height);
}

void OpenGLEventHandler::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	else if(key == GLFW_KEY_A && action == GLFW_PRESS) {
		keys |= 1;
//		std::cout << "Key Press: " <<(char)key << std::endl;
	} else if(key == GLFW_KEY_D && action == GLFW_PRESS)
		keys |= 2;
	else if(key == GLFW_KEY_W && action == GLFW_PRESS)
		keys |= 4;
	else if(key == GLFW_KEY_S && action == GLFW_PRESS)
		keys |= 8;
	else if(key == GLFW_KEY_C && action == GLFW_PRESS)
		keys |= 16;
	else if(key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		keys |= 32;
	else if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
		keys |= 64;
	else if(key == GLFW_KEY_HOME && action == GLFW_PRESS) {
		//position = glm::vec3(0, (CY + 1) * 0.1f, 0);
		position = glm::vec3(0, CY + 1, 0);
		angle = glm::vec3(0, -0.5, 0);
		update_vectors();
	} else if(key == GLFW_KEY_END && action == GLFW_PRESS) {
		//position = glm::vec3(0, (CX * SCX) * 0.1f, 0);
		position = glm::vec3(0, CX * SCX, 0);
		angle = glm::vec3(0, -M_PI * 0.49, 0);
		update_vectors();
	} else if(key == GLFW_KEY_A && action == GLFW_RELEASE) {
		keys &= ~1;
//		std::cout << "Key Release: " <<(char)key << std::endl;
	} else if(key == GLFW_KEY_D && action == GLFW_RELEASE)
		keys &= ~2;
	else if(key == GLFW_KEY_W && action == GLFW_RELEASE)
		keys &= ~4;
	else if(key == GLFW_KEY_S && action == GLFW_RELEASE)
		keys &= ~8;
	else if(key == GLFW_KEY_C && action == GLFW_RELEASE)
		keys &= ~16;
	else if(key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
		keys &= ~32;
	else if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
		keys &= ~32;
}

void OpenGLEventHandler::cursor_position_callback(GLFWwindow* window, double x, double y) {
	static bool wrap = false;
	static const float mousespeed = 0.001;

	if(!wrap) {
		angle.x -= (x - scr_width / 2) * mousespeed;
		angle.y -= (y - scr_height / 2) * mousespeed;

		if(angle.x < -M_PI)
			angle.x += M_PI * 2;
		if(angle.x > M_PI)
			angle.x -= M_PI * 2;
		if(angle.y < -M_PI / 2)
			angle.y = -M_PI / 2;
		if(angle.y > M_PI / 2)
			angle.y = M_PI / 2;

		update_vectors();

		wrap = true;
		glfwSetCursorPos(window, scr_width / 2, scr_height / 2);
	} else {
		wrap = false;
	}
}
