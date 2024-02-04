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
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, framebuffer.depthBuffer, 0);
		
		//Check if framebuffer is complete, if not output error code
		auto framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "FRAMEBUFFER ERROR: " << framebufferStatus << std::endl;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return framebuffer;
	}
}