#include <iostream>
#include <cstdlib>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "opengl_graphics_library.h"

#include "../global.h"
#include "../window/opengl_window.h"

void OpenGLGraphicsLibrary::InitGraphicsLibrary(Window* w) {


	GLFWwindow* win;
	// glfw: initialize and comfigure
	if(!glfwInit()) {
		std::cout << "GLFW initialize failed!\n"; 
		glfwTerminate();
		exit(-1);
	}

	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Comment out for Legacy OpenGL
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Comment out for Legacy OpenGL

	win = glfwCreateWindow(scr_width, scr_height, "Voxel Engine", NULL, NULL);

	// glfw window creation
	w->SetOpenGLWindow(win);
	if(w->GetWindow() == NULL) {
		std::cout << "Failed to create GLFW window\n";
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(w->GetWindow());

	// OpenGL States
//	glClearColor(1.0f, 0.6f, 0.2f, 1.0f);
	glClearColor(0.6f, 0.8f, 1.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);

	// Allow modern extentions features
	if(glewInit() != GLEW_OK) {
		std::cout << "GLEW initialisation failed!\n";
		glfwDestroyWindow(w->GetWindow());
		glfwTerminate();
		exit(-1);
	}

	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

	std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << "\n";
}
