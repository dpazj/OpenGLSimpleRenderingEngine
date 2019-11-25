#include "DynamicCubemap.h"
#include "glm/gtc/matrix_transform.hpp"




DynamicCubemap::DynamicCubemap(GLuint size)
{
	m_size = size;
	//create the cube map
	glGenTextures(1, &m_cube_map);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cube_map);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for (int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	}

	m_frame_buffer = new FrameBuffer();
	m_frame_buffer->Bind();
	m_frame_buffer->AddRenderBuffer(size, size);
	m_frame_buffer->AttachColour(m_cube_map);
	m_frame_buffer->CheckFrameBufferStatus();
	m_frame_buffer->Unbind();
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

}

void DynamicCubemap::RenderCubemap(glm::vec3 world_pos, std::function<void(glm::mat4, glm::mat4)> render_scene)
{
	glViewport(0, 0, m_size, m_size);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cube_map);
	m_frame_buffer->Bind();
	


	for (int face = 0; face < 6; face++)
	{
		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, m_cube_map, 0);
		m_frame_buffer->Clear();
		m_frame_buffer->CheckFrameBufferStatus();

		
		const glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);


		GLfloat pitch, yaw;
		glm::mat4 view(1.0f);
		switch (face)
		{
			
		case 0:
			view = glm::lookAt(world_pos, world_pos + glm::vec3(1,0,0), glm::vec3(0, -1, 0));
			break;
		case 1:
			view = glm::lookAt(world_pos, world_pos + glm::vec3(-1,0,0), glm::vec3(0, -1, 0));
			break;
		case 2:
			view = glm::lookAt(world_pos, world_pos + glm::vec3(0,1,0), glm::vec3(0, 0, 1));
			break;
		case 3:
			view = glm::lookAt(world_pos, world_pos + glm::vec3(0,-1,0), glm::vec3(0, 0, -1));
			break;
		case 5:
			view = glm::lookAt(world_pos, world_pos + glm::vec3(0, 0,-1), glm::vec3(0,-1,0)); 
			break;
		case 4:
			view = glm::lookAt(world_pos, world_pos + glm::vec3(0, 0, 1), glm::vec3(0, -1, 0)); 
			break;
			
		};
		render_scene(projection,view);
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	m_frame_buffer->Unbind();
}

void DynamicCubemap::BindCubemap()
{
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cube_map);
}