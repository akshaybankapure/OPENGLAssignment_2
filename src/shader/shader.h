#ifndef SHADER_H
#define SHADER_H

class Shader {
public:
	/* Init Shader */
	virtual void InitShader(const char* vertex_path,
                          const char* fragment_path,
                          const char* geometry_path) = 0;

	/* Activate the shader */
	virtual void Use() = 0;
};

#endif
