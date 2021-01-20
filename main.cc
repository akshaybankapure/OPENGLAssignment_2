#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <time.h>

#include <iostream>

#include "src/chunk/chunk.h"
#include "src/event_handler/opengl_event_handler.h"
#include "src/graphics_library/opengl_graphics_library.h"
#include "src/renderer/opengl_renderer.h"
#include "src/shader/opengl_shader.h"
#include "src/window/opengl_window.h"

/* Global Declarations */
OpenGLShader* global_shader = NULL;

SuperChunk *world;

GLint attribute_coord;
GLint uniform_mvp;
GLint uniform_texture;
GLuint texture;

glm::vec3 position;
glm::vec3 forward;
glm::vec3 right;
glm::vec3 up;
glm::vec3 lookat;
glm::vec3 angle;

time_t now;
unsigned int keys;

unsigned int scr_width = 800;
unsigned int scr_height = 600;

glm::vec3 cube_center(0.0f, 0.0f, 0.0f);
float cube_scale = 1.0f;

int game_state = 1;

#define PLAY_STATE 0
#define MENU_STATE 1

int main() {
	std::cout << "Init Graphics.\n";

	// Init graphics parameters
	Window *w = new OpenGLWindow();
	GraphicsLibrary* gl = new OpenGLGraphicsLibrary();
	gl->InitGraphicsLibrary(w);

	// Shader loading and compile
	global_shader = new OpenGLShader("res/shaders/glescraft.v.glsl", "res/shaders/glescraft.f.glsl");
	
	

	EventHandler* eh = new OpenGLEventHandler();
	eh->InitializeEventHandler(w);

	// Rander
	Renderer* renderer = new OpenGLRenderer();

	int render_distance = 32;
	int i = 0;

	glClearColor(0.6f, 0.8f, 1.0f, 0.0f);

	eh->InitializeCursor(w);

	int seed = i;

	world = new SuperChunk(seed, render_distance);
	world->init_resorces();

	// Rander loop
	renderer->StartRenderLoop(w);

	return 0;
}
