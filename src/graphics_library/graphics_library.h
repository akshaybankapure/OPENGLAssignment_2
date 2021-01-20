#ifndef GRAPHICS_LIBRARY_H
#define GRAPHICS_LIBRARY_H

#include "../window/window.h"

class GraphicsLibrary {
public:
	/* Init Graphics Library */
	virtual void InitGraphicsLibrary(Window *w) = 0;
};

#endif
