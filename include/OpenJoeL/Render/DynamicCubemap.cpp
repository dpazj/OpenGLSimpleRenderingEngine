#include "DynamicCubemap.h"
#include "glm/gtc/matrix_transform.hpp"




DynamicCubemap::DynamicCubemap(GLuint width, GLuint height)
{
	//create the cube map
	glGenTextures(1, &m_cube_map);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cube_map);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	}

	m_frame_buffer = new FrameBuffer();
	m_frame_buffer->AddRenderBuffer(m_cube_map, width, height);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void DynamicCubemap::RenderCubemap(glm::vec3 world_pos, std::function<void(glm::mat4)> render_scene)
{

	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cube_map);
	m_frame_buffer->Bind();
	m_frame_buffer->Clear();

	for (int face = 0; face < 6; face++)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + (int)face, m_cube_map, 0);
		glm::mat4 view(1.0f);
		switch (face)
		{
		case 1:
			view = glm::lookAt(world_pos, glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));
			break;
		case 2:
			view = glm::lookAt(world_pos, glm::vec3(-1, 0, 0), glm::vec3(0, 1, 0));
			break;
		case 3:
			view = glm::lookAt(world_pos, glm::vec3(0, 10, 0), glm::vec3(1, 0, 0));
			break;
		case 4:
			view = glm::lookAt(world_pos, glm::vec3(0, -10, 0), glm::vec3(1, 0, 0));
			break;
		case 5:
			view = glm::lookAt(world_pos, glm::vec3(0, 0, 10), glm::vec3(0, 1, 0));
			break;
		case 6:
			view = glm::lookAt(world_pos, glm::vec3(0, 0, -10), glm::vec3(0, 1, 0));
			break;
		default:
			break;
		};
		render_scene(view);
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	m_frame_buffer->Unbind();
}