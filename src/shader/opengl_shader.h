#ifndef OPENGL_SHADER_H
#define OPENGL_SHADER_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>

#include "shader.h"

class OpenGLShader : public Shader {
public:
	/* Constructor */
	OpenGLShader() {}
	OpenGLShader(const char* vertex_path, const char* fragment_path, const char* geometry_path = nullptr) {
		InitShader(vertex_path, fragment_path, geometry_path);
	}

	/* Init Shader */
	void InitShader(const char* vertex_path, const char* fragment_path, const char* geometry_path);

	/* Activate the shader */
	void Use() {
		glUseProgram(id_);
	}

	/* Get Attribute */
	GLint GetAttrib(const char *name);

	/* Get Uniform */
	GLint GetUniform(const char *name);

	/* Utility uniform functions */

	void SetBool(const std::string &name, bool value) const;
	void SetInt(const std::string &name, int value) const;
	void SetFloat(const std::string &name, float value) const;
	void SetVec2(const std::string &name, glm::vec2 &value) const;
	void SetVec2(const std::string &name, float x, float y) const;
	void SetVec3(const std::string &name, glm::vec3 &value) const;
	void SetVec3(const std::string &name, float x, float y, float z) const;
	void SetVec4(const std::string &name, glm::vec4 &value) const;
	void SetVec4(const std::string &name, float x, float y, float z, float w) const;
	void SetMat2(const std::string &name, glm::mat2 &mat) const;
	void SetMat3(const std::string &name, glm::mat3 &mat) const;
	void SetMat4(const std::string &name, glm::mat4 &mat) const;

private:	
  unsigned int id_;

	/* Utility function for checking shader compilation linking errors. */
	void check_compile_errors(GLuint shader, std::string type);
};

#endif
