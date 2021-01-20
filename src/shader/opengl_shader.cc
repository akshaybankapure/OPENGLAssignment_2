#include <fstream>
#include <sstream>
#include <iostream>

#include "opengl_shader.h"

void OpenGLShader::InitShader(const char* vertex_path, const char* fragment_path, const char* geometry_path = nullptr) {
	// 1. Retrive the vertex/fragment source code from  filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;

	// Ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		// open files
		vShaderFile.open(vertex_path);
		fShaderFile.open(fragment_path);
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// Close the file handler
		vShaderFile.close();
		fShaderFile.close();
		// concert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		// if geometry shader path is present, also load a geometry shaders
		if(geometry_path != nullptr) {
			gShaderFile.open(geometry_path);
			std::stringstream gShaderStream;
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryCode = gShaderStream.str(); 
		}
	} catch (std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	// 2. Compile Shaders
	unsigned int vertex, fragment;
	// Vertex shader
	vertex  = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	check_compile_errors(vertex, "VERTEX");

	// Fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	check_compile_errors(fragment, "FRAGMENT");

	// If geometry shader is given, compile geometry shader
	unsigned int geometry;
	if(geometry_path != nullptr) {
		const char* gShaderCode = geometryCode.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		check_compile_errors(geometry, "GEOMETRY");
	}
	
	// Shader program
	id_ = glCreateProgram();
	glAttachShader(id_, vertex);
	glAttachShader(id_, fragment);
	if(geometry_path != nullptr)
		glAttachShader(id_, geometry);
	glLinkProgram(id_);
	check_compile_errors(id_, "PROGRAM");
	// Delete the shaders as they are linked into program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if(geometry_path != nullptr)
		glDeleteShader(geometry);
}

GLint OpenGLShader::GetAttrib(const char *name) {
	GLint attribute = glGetAttribLocation(id_, name);
	if(attribute == -1)
		std::cout << "ERROR::SHADER::Could not bind attribute " << name << "\n";
	return attribute;
}

GLint OpenGLShader::GetUniform(const char *name) {
	GLint uniform = glGetUniformLocation(id_, name);
	if(uniform == -1)
		std::cout << "ERROR::SHADER::Could not bind uniform " << name << "\n";
	return uniform;
}

void OpenGLShader::SetBool(const std::string &name, bool value) const {
	glUniform1i(glGetUniformLocation(id_, name.c_str()), (int)value);
}

void OpenGLShader::SetInt(const std::string &name, int value) const {
	glUniform1i(glGetUniformLocation(id_, name.c_str()), value);
}

void OpenGLShader::SetFloat(const std::string &name, float value) const {
	glUniform1f(glGetUniformLocation(id_, name.c_str()), value);
}

void OpenGLShader::SetVec2(const std::string &name, glm::vec2 &value) const {
	glUniform2fv(glGetUniformLocation(id_, name.c_str()), 1, &value[0]);
}

void OpenGLShader::SetVec2(const std::string &name, float x, float y) const {
		glUniform2f(glGetUniformLocation(id_, name.c_str()), x, y);
}

void OpenGLShader::SetVec3(const std::string &name, glm::vec3 &value) const {
	glUniform3fv(glGetUniformLocation(id_, name.c_str()),  1, &value[0]);
}

void OpenGLShader::SetVec3(const std::string &name, float x, float y, float z) const {
		glUniform3f(glGetUniformLocation(id_, name.c_str()), x, y, z);
}

void OpenGLShader::SetVec4(const std::string &name, glm::vec4 &value) const {
	glUniform4fv(glGetUniformLocation(id_, name.c_str()), 1, &value[0]);
}

void OpenGLShader::SetVec4(const std::string &name, float x, float y, float z, float w) const {
		glUniform4f(glGetUniformLocation(id_, name.c_str()), x, y, z, w);
}

void OpenGLShader::SetMat2(const std::string &name, glm::mat2 &mat) const {
	glUniformMatrix2fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void OpenGLShader::SetMat3(const std::string &name, glm::mat3 &mat) const {
	glUniformMatrix3fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void OpenGLShader::SetMat4(const std::string &name, glm::mat4 &mat) const {
	glUniformMatrix4fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void OpenGLShader::check_compile_errors(GLuint shader, std::string type) {
	GLint success;
	GLchar info_log[1024];
	if(type != "PROGRAM") {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if(!success) {
			glGetShaderInfoLog(shader, 1024, NULL, info_log);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << info_log << "\n-- ---------------------------------------------------" << std::endl;
		}
	} else {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if(!success) {
			glGetProgramInfoLog(shader, 1024, NULL, info_log);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << info_log << "\n-- ---------------------------------------------------" << std::endl;
		}
	}
}
