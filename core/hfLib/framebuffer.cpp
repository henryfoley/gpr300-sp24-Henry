#include "framebuffer.h"


namespace hfLib {
	Framebuffer hfLib::createFramebuffer(unsigned int width, unsigned int height, int colorFormat)
	{
		//Create Framebuffer
		Framebuffer framebuffer;

		//Frame Buffer Configuration
		glCreateFramebuffers(1, &framebuffer.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);

		//Create Single Color Buffer
		glGenTextures(1, &framebuffer.colorBuffer);
		glBindTexture(GL_TEXTURE_2D, framebuffer.colorBuffer);
		glTexStorage2D(GL_TEXTURE_2D, 1, colorFormat, width, height);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, framebuffer.colorBuffer, 0);

		//Multiple Color Buffer
		/*glGenTextures(8, framebuffer.colorBuffers);
		for (unsigned int i = 0; i < 8; i++)
		{
			glBindTexture(GL_TEXTURE_2D, framebuffer.colorBuffers[i]);
			glTexStorage2D(GL_TEXTURE_2D, 1, colorFormat, width, height);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, framebuffer.colorBuffers[i], 0);
		}*/

		//Depth Buffer and Stencil Buffer as Render Buffer

		/*glGenRenderbuffers(1, &framebuffer.depthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, framebuffer.depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, framebuffer.depthBuffer);*/

		//Depth Buffer
		glGenTextures(1, &framebuffer.depthBuffer);
		glBindTexture(GL_TEXTURE_2D, framebuffer.depthBuffer);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT16, width, height);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, framebuffer.depthBuffer, 0);

		//Check if framebuffer is complete, if not output error code
		auto framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "FRAMEBUFFER ERROR: " << framebufferStatus << std::endl;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return framebuffer;
	}

	Framebuffer hfLib::createFramebuffer(unsigned int width, unsigned int height)
	{
		//Create Framebuffer
		Framebuffer framebuffer;
		
		//Frame Buffer Configuration
		glCreateFramebuffers(1, &framebuffer.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);

		//Create Single Depth Buffer
		glGenTextures(1, &framebuffer.depthBuffer);
		glBindTexture(GL_TEXTURE_2D, framebuffer.depthBuffer);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT16, width, height);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[4] = { 1.0f,1.0f,1.0f,1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, framebuffer.depthBuffer, 0);

		//Tell OpenGL not to render any Color Data
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//Check if framebuffer is complete, if not output error code
		auto framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "FRAMEBUFFER ERROR: " << framebufferStatus << std::endl;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return framebuffer;
	}

	Framebuffer hfLib::createGBuffer(unsigned int width, unsigned int height)
	{
		Framebuffer framebuffer;
		framebuffer.width = width;
		framebuffer.height = height;

		//Create Framebuffer
		glCreateFramebuffers(1, &framebuffer.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);

		int formats[3] = { 
			GL_RGBA32F,		//0 = World Position
			GL_RGB16F,		//1 = World Normal
			GL_RGB16F		//2 = Albedo
		};

		//Create Multiple Color Buffers
		for (size_t i = 0; i < 3; i++)
		{
			glGenTextures(1, &framebuffer.colorBuffers[i]);
			glBindTexture(GL_TEXTURE_2D, framebuffer.colorBuffers[i]);
			glTexStorage2D(GL_TEXTURE_2D, 1, formats[i], width, height);
			//Clamp to Border
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			//Attach each texture to different slot
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, framebuffer.colorBuffers[i], 0);
			//Explicitly tell OpenGL which color attachment to use
			const GLenum drawBuffers[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
			glDrawBuffers(3, drawBuffers);
		}

		//Create Single Depth Buffer
		glGenTextures(1, &framebuffer.depthBuffer);
		glBindTexture(GL_TEXTURE_2D, framebuffer.depthBuffer);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT16, width, height);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[4] = { 1.0f,1.0f,1.0f,1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, framebuffer.depthBuffer, 0);

		//Check if framebuffer is complete, if not output error code
		auto framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "FRAMEBUFFER ERROR: " << framebufferStatus << std::endl;
		}

		//Clean up global state
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return framebuffer;
	}
}