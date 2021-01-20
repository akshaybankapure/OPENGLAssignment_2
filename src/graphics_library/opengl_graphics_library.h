#ifndef OPENGL_GRAPHICS_LIBRARY_H
#define OPENGL_GRAPHICS_LIBRARY_H

#include "graphics_library.h"

class OpenGLGraphicsLibrary : public GraphicsLibrary {
public:
	OpenGLGraphicsLibrary() {}

	/* Init Graphics Library */
	void InitGraphicsLibrary(Window *w);

};

#endif
