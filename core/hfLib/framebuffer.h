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
		unsigned int colorBuffer[8];
		unsigned int depthBuffer;
		unsigned int width;
		unsigned int height;
	};
	Framebuffer createFramebuffer(unsigned int width, unsigned int height, int colorFormat);
}