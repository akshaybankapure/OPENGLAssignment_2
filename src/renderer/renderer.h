#ifndef RENDERER_H
#define RENDERER_H

#include "../window/window.h"

class Renderer {
public:
	/* Init Graphics Library */
	virtual void StartRenderLoop(Window *w) = 0;
};

#endif
