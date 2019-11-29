#pragma once 

#include "OpenJoeL/Render/FrameBuffer.h"
#include <functional>
#include "glm/gtc/matrix_transform.hpp"


class PointShadowMap
{
public:
	PointShadowMap(GLfloat size)
	{
		m_size = size;
		glGenTextures(1, &m_cube_map);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_cube_map);
		for (unsigned int i = 0; i < 6; ++i) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, m_size, m_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


		m_frame_buffer = new FrameBuffer();
		m_frame_buffer->Bind();
		m_frame_buffer->AttachTexture(m_cube_map);
		m_frame_buffer->RemoveDrawBuffer();
		m_frame_buffer->RemoveReadBuffer();
		m_frame_buffer->CheckFrameBufferStatus();
		m_frame_buffer->Unbind();
	}

	void RenderShadowMap(glm::vec3 world_pos, std::function<void(glm::mat4, glm::mat4, glm::vec3)> render_scene, Shader * shader)
	{
		glViewport(0, 0, m_size, m_size);

		m_frame_buffer->Bind();
		m_frame_buffer->Clear();

		glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, far_plane);

		transformations.clear();
		transformations.push_back(projection *	glm::lookAt(world_pos, world_pos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		transformations.push_back(projection *	glm::lookAt(world_pos, world_pos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		transformations.push_back(projection *	glm::lookAt(world_pos, world_pos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
		transformations.push_back(projection *	glm::lookAt(world_pos, world_pos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
		transformations.push_back(projection *	glm::lookAt(world_pos, world_pos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		transformations.push_back(projection *	glm::lookAt(world_pos, world_pos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

		shader->UseShader();
		UpdateShaderWithShadowMatricies(shader);
		shader->SetVec3("light_pos", world_pos);
		shader->SetFloat("far_plane", far_plane);


		render_scene(glm::mat4(1.0f), glm::mat4(1.0f), world_pos);
		m_frame_buffer->Unbind();
	}

	void UpdateShaderWithShadowMatricies(Shader* shader)
	{
		shader->UseShader();
		for (unsigned int i = 0; i < 6; ++i) {
			shader->SetMat4(("shadowMatrices[" + std::to_string(i) + "]").c_str(), transformations[i]);
		}
	}

	void BindShadowMap(GLuint index)
	{
		glActiveTexture(GL_TEXTURE10 + index);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_cube_map);
	}

	GLfloat GetFarPlane()
	{
		return far_plane;
	}

private:
	std::vector<glm::mat4> transformations;

	FrameBuffer* m_frame_buffer;
	GLfloat m_size;
	GLuint m_cube_map;
	GLfloat far_plane = 300.0f;
};



class DirectionalShadowMap {

public:

	DirectionalShadowMap(GLfloat size)
	{
		m_size = size;
		glGenTextures(1, &m_depth_map);
		glBindTexture(GL_TEXTURE_2D, m_depth_map);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		m_frame_buffer = new FrameBuffer();
		m_frame_buffer->Bind();
		m_frame_buffer->Attach2DTexture(m_depth_map, GL_DEPTH_ATTACHMENT);
		m_frame_buffer->RemoveDrawBuffer();
		m_frame_buffer->RemoveReadBuffer();
		m_frame_buffer->CheckFrameBufferStatus();
		m_frame_buffer->Unbind();
	}


	void RenderShadowMap(glm::vec3 world_pos, std::function<void(glm::mat4, glm::mat4)> render_scene)
	{

		glViewport(0, 0, m_size, m_size);
		

		m_frame_buffer->Bind();
		m_frame_buffer->Clear();
		
		m_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		m_view = glm::lookAt(world_pos, -world_pos, glm::vec3(0,1,0)); //change middle vector to where the light is looking at.

		render_scene(m_projection, m_view);

		
		m_frame_buffer->Unbind();
	}

	glm::mat4 GetMatrix()
	{
		return m_projection * m_view;
	}

	void BindShadowMap()
	{
		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_2D, m_depth_map);
	}

	GLfloat GetFarPlane()
	{
		return far_plane;
	}

	GLuint m_depth_map = 0;

private:
	GLfloat m_size = 0; 
	
	FrameBuffer* m_frame_buffer;
	
	GLfloat near_plane = 1.0f;
	GLfloat far_plane = 7.5f;
	glm::mat4 m_projection;
	glm::mat4 m_view;

};