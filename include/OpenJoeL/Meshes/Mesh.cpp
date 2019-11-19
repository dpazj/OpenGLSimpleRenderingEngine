#include "Mesh.h"

Mesh::Mesh()
{
	m_attribute_index = -1;
	glGenVertexArrays(1, &m_mesh_vao);
}

void Mesh::Draw(Shader* shader)
{
	m_textures.BindTexures(shader);

	glBindVertexArray(m_mesh_vao);
	BindBuffers();


	if (m_has_custom_draw)
	{
		m_custom_draw();
	}
	else
	{
		if (m_has_indices)
		{
			glDrawElements(GL_TRIANGLES, m_indice_count, GL_UNSIGNED_INT, (GLvoid*)(0));
		}
		else
		{
			glDrawArrays(GL_TRIANGLES, 0, m_vertice_count);
		}
	}

	

	glBindVertexArray(0);
}

void Mesh::SetMeshTextures(MeshTextures textures)
{
	m_textures = textures;
}


void Mesh::BindBuffers()
{
	if (m_attribute_v_position != -1)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_position_buffer_object);
		glEnableVertexAttribArray(m_attribute_v_position);
		glVertexAttribPointer(m_attribute_v_position, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	if (m_attribute_v_texture_cords != -1)
	{
		glEnableVertexAttribArray(m_attribute_v_texture_cords);
		glBindBuffer(GL_ARRAY_BUFFER, m_texture_buffer_object);
		glVertexAttribPointer(m_attribute_v_texture_cords, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}

	if (m_attribute_v_normal != -1)
	{
		glEnableVertexAttribArray(m_attribute_v_normal);
		glBindBuffer(GL_ARRAY_BUFFER, m_normals_buffer_object);
		glVertexAttribPointer(m_attribute_v_normal, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	if (m_has_indices)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_element_buffer_object);
	}
}


GLuint Mesh::GetNewAttributeCord()
{
	m_attribute_index++;
	return m_attribute_index;
}

void Mesh::SetCustomDraw(std::function<void()> draw_fun)
{
	m_has_custom_draw = true;
	m_custom_draw = draw_fun;
}