#pragma once
#include "../Texture/Texture.h"
#include "../Shaders/Shader.h"

#include <vector>
#include <functional>
#include <glload/gl_4_0.h>


class Mesh {

public:
	
	Mesh();
	void Draw(Shader* shader);
	void SetMeshTextures(MeshTextures textures);
	Mesh GetMesh();
	

protected:
	
	template <typename T>
	void SetVertexPoints(std::vector<T> vertex_positions)
	{
		glBindVertexArray(m_mesh_vao);

		glGenBuffers(1, &m_position_buffer_object);
		glBindBuffer(GL_ARRAY_BUFFER, m_position_buffer_object);
		glBufferData(GL_ARRAY_BUFFER, vertex_positions.size() * sizeof(T), vertex_positions.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		m_vertice_count = vertex_positions.size() / 3;
		m_attribute_v_position = GetNewAttributeCord();

		glBindVertexArray(0);
	}

	template <typename T>
	void SetTextureCords(std::vector<T> texture_cords)
	{
		glBindVertexArray(m_mesh_vao);

		glGenBuffers(1, &m_texture_buffer_object);
		glBindBuffer(GL_ARRAY_BUFFER, m_texture_buffer_object);
		glBufferData(GL_ARRAY_BUFFER, texture_cords.size() * sizeof(T), texture_cords.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		m_attribute_v_texture_cords = GetNewAttributeCord();
		glBindVertexArray(0);
	}

	template <typename T>
	void SetNormals(std::vector<T> normals)
	{
		glBindVertexArray(m_mesh_vao);

		glGenBuffers(1, &m_normals_buffer_object);
		glBindBuffer(GL_ARRAY_BUFFER, m_normals_buffer_object);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(T), normals.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		m_attribute_v_normal = GetNewAttributeCord();

		glBindVertexArray(0);
	}

	template <typename T>
	void SetIndices(std::vector<T> indices)
	{
		m_has_indices = true;
		m_indice_count = indices.size();
		glBindVertexArray(m_mesh_vao);

		glGenBuffers(1, &m_element_buffer_object);
		glBindBuffer(GL_ARRAY_BUFFER, m_element_buffer_object);
		glBufferData(GL_ARRAY_BUFFER, indices.size() * sizeof(T), indices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		m_has_indices = true;
		glBindVertexArray(0);
	}

	void SetCustomDraw(std::function<void()> draw_fun);

	

protected:
	GLuint m_indice_count = 0;

private:
	
	MeshTextures m_textures;

	void BindBuffers();
	GLuint GetNewAttributeCord();

	GLuint m_attribute_index;

	GLuint m_element_buffer_object = 0;
	GLuint m_position_buffer_object = 0;
	GLuint m_texture_buffer_object = 0;
	GLuint m_normals_buffer_object = 0;
	GLuint m_mesh_vao = 0;

	GLuint m_attribute_v_position = -1;
	GLuint m_attribute_v_texture_cords = -1;
	GLuint m_attribute_v_normal = -1;

	GLuint m_vertice_count = 0;

	bool m_has_indices = false;
	bool m_has_custom_draw = false;
	std::function<void()> m_custom_draw;
	
};
