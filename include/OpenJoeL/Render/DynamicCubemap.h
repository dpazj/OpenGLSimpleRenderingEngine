//Joe Riemersma

#pragma once

#include "OpenJoeL/Shaders/Shader.h"
#include "FrameBuffer.h"

#include <glload/gl_4_0.h>
#include <functional>

class DynamicCubemap
{
public:

	DynamicCubemap(GLuint size);
	void RenderCubemap(glm::vec3 world_position, std::function<void(glm::mat4,glm::mat4)> render_scene);

	void BindCubemap();
	

private:

	FrameBuffer * m_frame_buffer;

	GLuint m_cube_map = 0;

	GLfloat m_size;
};