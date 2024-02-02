#pragma once

/*
*	Author: Henry Foley
*/

#include <GLFW/glfw3.h>
//#include "external/glad.h"

namespace hfLib {
	struct Framebuffer
	{
		unsigned int fbo;
		unsigned int colorBuffer[8];
		unsigned int depthBuffer;
		unsigned int width;
		unsigned int height;
	};
	Framebuffer createFramebuffer(unsigned int width, unsigned int height, int colorFormat);
}