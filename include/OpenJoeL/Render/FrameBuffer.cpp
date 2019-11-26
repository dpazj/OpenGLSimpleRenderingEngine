#include "FrameBuffer.h"
#include <iostream>

FrameBuffer::FrameBuffer()
{
	glGenFramebuffers(1, &m_fbo);
}


void FrameBuffer::AddRenderBuffer(GLuint width, GLuint height)
{
	
	glGenRenderbuffers(1, &m_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_fbo);
	
}


void FrameBuffer::AttachTexture(GLuint texture, GLenum attachment ,GLenum target)
{
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, target, texture, 0);
}

void FrameBuffer::RemoveDrawBuffer() { glDrawBuffer(GL_NONE); }
void FrameBuffer::RemoveReadBuffer() { glReadBuffer(GL_NONE); }

void FrameBuffer::CheckFrameBufferStatus()
{

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch (status) {
		case GL_FRAMEBUFFER_UNDEFINED: {
			fprintf(stderr, "Undefined.\n");
			break;
		}
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: {
			fprintf(stderr, "FBO: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT.\n");
			break;
		}
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: {
			fprintf(stderr, "FBO: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT.\n");
			break;
		}
		case GL_FRAMEBUFFER_UNSUPPORTED: {
			fprintf(stderr, "FBO: GL_FRAMEBUFFER_UNSUPPORTED.\n");
			break;
		}
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: {
			fprintf(stderr, "FBO: GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE.\n");
			break;
		}
	}

}


void FrameBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void FrameBuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


//void FrameBuffer::Init()
//{
//	glEnable(GL_DEPTH_TEST);
//	float quadVertices[] = { // 
//		// positions   // texCoords
//		-1.0f,  1.0f,  0.0f, 1.0f,
//		-1.0f, -1.0f,  0.0f, 0.0f,
//		 1.0f, -1.0f,  1.0f, 0.0f,
//
//		-1.0f,  1.0f,  0.0f, 1.0f,
//		 1.0f, -1.0f,  1.0f, 0.0f,
//		 1.0f,  1.0f,  1.0f, 1.0f
//	};
//
//	glGenVertexArrays(1, &m_vao);
//	glGenBuffers(1, &m_vbo);
//	glBindVertexArray(m_vao);
//	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
//
//	m_frame_buffer_shader->UseShader();
//	m_frame_buffer_shader->SetInt("screenTexture", 0);

//	//create framebuffer obj
//	glGenFramebuffers(1, &m_framebuffer_object);
//	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer_object);
//
//	//generate frambuffer texture object
//	glActiveTexture(GL_TEXTURE0);
//	glGenTextures(1, &m_texture_colour_buffer);
//	glBindTexture(GL_TEXTURE_2D, m_texture_colour_buffer);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glBindTexture(GL_TEXTURE_2D, 0);
//
//	// attach it to currently bound framebuffer object
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture_colour_buffer, 0);
//
//	glGenRenderbuffers(1, &m_render_buffer_object);
//	glBindRenderbuffer(GL_RENDERBUFFER, m_render_buffer_object);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
//	glBindRenderbuffer(GL_RENDERBUFFER, 0);
//
//	//attach renderbuffer object
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_render_buffer_object);
//
//	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//}
//
//void FrameBuffer::FirstRender()
//{
//	
//	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer_object);
//	glEnable(GL_DEPTH_TEST); 
//
//	//clear the framebuffer's content
//	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//}
//
//void FrameBuffer::SecondRender()
//{
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
//	// clear all relevant buffers
//	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
//	glClear(GL_COLOR_BUFFER_BIT);
//
//	m_frame_buffer_shader->UseShader();
//	glBindVertexArray(m_vao);
//	glBindTexture(GL_TEXTURE_2D, m_texture_colour_buffer);	// use the color attachment texture as the texture of the quad plane
//	glDrawArrays(GL_TRIANGLES, 0, 6);
//}