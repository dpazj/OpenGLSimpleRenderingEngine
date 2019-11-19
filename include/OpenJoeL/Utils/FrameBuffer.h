#pragma once

#include "../Shaders/Shader.h"
#include <memory>
#include <glm/glm.hpp>
#include <glload/gl_4_0.h>
#include <glload/gl_load.h>


class FrameBuffer
{
public:

	FrameBuffer(GLint width, GLint height, std::shared_ptr<Shader> shader);
	void Init();

	void FirstRender();
	void SecondRender();


private:
	std::shared_ptr<Shader> m_frame_buffer_shader;

	GLuint m_framebuffer_object = 0;
	GLuint m_texture_colour_buffer = 0;
	GLuint m_render_buffer_object = 0;

	GLint m_width;
	GLint m_height;

	//quad
	GLuint m_vao = 0;
	GLuint m_vbo = 0;


};