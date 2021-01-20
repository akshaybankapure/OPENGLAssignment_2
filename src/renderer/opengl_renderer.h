#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H

#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "renderer.h"

class OpenGLRenderer : public Renderer {
private:
	void render();

	void update_state();

public:
	void StartRenderLoop(Window *w);
};

#endif
