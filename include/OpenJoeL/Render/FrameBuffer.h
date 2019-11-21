#pragma once

#include <glm/glm.hpp>
#include <glload/gl_4_0.h>
#include <glload/gl_load.h>


class FrameBuffer
{
public:

	FrameBuffer();
	void Bind();
	void Unbind();
	void Clear();
	void AddRenderBuffer(GLuint texture, GLuint width, GLuint height);


private:
	
	GLuint m_fbo = 0;
	GLuint m_rbo = 0;

};