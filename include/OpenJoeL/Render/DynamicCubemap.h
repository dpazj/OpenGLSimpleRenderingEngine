//Joe Riemersma

#pragma once

#include "FrameBuffer.h"

#include <glload/gl_4_0.h>
#include <functional>

class DynamicCubemap
{
public:

	DynamicCubemap(GLuint width, GLuint height);
	void RenderCubemap(glm::vec3 world_position, std::function<void(glm::mat4)> render_scene);

	

private:

	FrameBuffer * m_frame_buffer;

	GLuint m_cube_map = 0;

};