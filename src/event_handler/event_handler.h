#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include "../window/window.h"

class EventHandler {
public:
	virtual void InitializeEventHandler(Window *w) = 0; 
	virtual void InitializeCursor(Window* w) = 0;
};

#endif
