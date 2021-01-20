#ifndef GLOBAL_H
#define GLOBAL_H

#include <glm/gtc/type_ptr.hpp>
#include <time.h>

#include "shader/opengl_shader.h"
#include "chunk/chunk.h"

extern OpenGLShader* global_shader;

extern SuperChunk *world;

extern GLint attribute_coord;
extern GLint uniform_mvp;
extern GLint uniform_texture;
extern GLuint texture;

extern glm::vec3 position;
extern glm::vec3 forward;
extern glm::vec3 right;
extern glm::vec3 up;
extern glm::vec3 lookat;
extern glm::vec3 angle;

extern time_t now;
extern unsigned int keys;

extern unsigned int scr_width;
extern unsigned int scr_height;

#endif
