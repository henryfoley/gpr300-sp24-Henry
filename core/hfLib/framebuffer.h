#pragma once

/*
*	Author: Henry Foley
*/

#include "../ew/external/glad.h"
#include <cstdlib>
#include <iostream>

namespace hfLib {
	struct Framebuffer
	{
		unsigned int fbo;
		unsigned int colorBuffer;
		unsigned int colorBuffers[3];
		unsigned int depthBuffer;
		unsigned int width;
		unsigned int height;
	};
	Framebuffer createFramebuffer(unsigned int width, unsigned int height, int colorFormat);
	Framebuffer createFramebuffer(unsigned int width, unsigned int height);
	Framebuffer createGBuffer(unsigned int width, unsigned int height);
}