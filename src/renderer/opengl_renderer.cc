#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>

#include "opengl_renderer.h"

#include "../constants/constants.h"
#include "../global.h"
#include "../window/opengl_window.h"

void OpenGLRenderer::render() {

#ifdef SHOW_FPS
	float currentFrame = glfwGetTime();
	static long int frames = 0;
	static float _lastFrame = 0.0f;
	float delta = currentFrame - _lastFrame;
	frames++;
	if(delta >= 0.2) {
		std::cout << "FPS: " << frames/(float)delta << "\n";
		frames = 0;
		_lastFrame = currentFrame;
	}
#endif
	world->Update();
	glm::mat4 view = glm::lookAt(position, position + lookat, up);
	glm::mat4 projection = glm::perspective(45.0f, 1.0f*scr_width/scr_height, 0.01f, 1000.0f);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));

	glm::mat4 mvp = projection * view * model;

	global_shader->SetMat4("mvp", mvp);
	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POLYGON_OFFSET_FILL);
	// Then draw chunks

	world->Render(mvp);

}

void OpenGLRenderer::update_state() {
	static float pt = 0;
	static const float movespeed = 50*BLOCK_SIZE;
	now = time(0);
	float t = glfwGetTime();
	float dt = t - pt;
	pt = t;

	if(keys & 1)
		position -= right * movespeed * dt;
	if(keys & 2)
		position += right * movespeed * dt;
	if(keys & 4)
		position += forward * movespeed * dt;
	if(keys & 8)
		position -= forward * movespeed * dt;
	if(keys & 16)
		position.y -= movespeed * dt;
	if(keys & 32)
		position.y += movespeed * dt;
}

void OpenGLRenderer::StartRenderLoop(Window *w) {
	while(!glfwWindowShouldClose(w->GetWindow())) {

		update_state();

		render();

		glfwSwapBuffers(w->GetWindow());
		glfwPollEvents();
	}
}

