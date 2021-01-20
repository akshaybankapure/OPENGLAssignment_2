#version 330 core

layout (location = 0) in vec4 coord;

out vec4 texcoord;

uniform mat4 mvp;

void main(void) {
	// Just pass the original vertex coordinates to the fragment shader as texture coordinates
	texcoord = coord;

	// Apply the model-view-projection matrix to the xyz components of the vertex coordinates
	gl_Position = mvp * vec4(coord.xyz, 1);
}
