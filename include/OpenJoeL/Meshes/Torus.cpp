///*
//Joe Riemersma
//30/10/10
//*/
//
//
////adapted from https://gamedev.stackexchange.com/questions/16845/how-do-i-generate-a-torus-mesh and http://www.mbsoftworks.sk/tutorials/opengl4/011-indexed-rendering-torus/
//
//#include "Torus.h"
//#include <iostream>
//
//Torus::Torus(GLfloat ring_radius, GLfloat tube_radius, GLint ring_segments, GLint tube_segments, GLfloat ring_repeats, GLfloat tube_repeats)
//{
//	m_ring_segments = ring_segments;
//	m_tube_segments = tube_segments;
//	m_tube_radius = tube_radius;
//	m_ring_radius = ring_radius;
//
//	m_ring_repeats = ring_repeats;
//	m_tube_repeats = tube_repeats;
//}
//
//void Torus::SetTextureSettings(GLfloat ring_repeats, GLfloat tube_repeats)
//{
//	m_ring_repeats = ring_repeats;
//	m_tube_repeats = tube_repeats;
//}
//
//void Torus::Init()
//{
//	
//
//	const int ring_steps = m_ring_segments + 1;
//	const int tube_steps = m_tube_segments + 1;
//
//	const GLfloat PI = 3.14159265358979324;
//	
//	const GLfloat tube_delta_angle = (float) (2 * PI / m_tube_segments);
//	const GLfloat ring_delta_angle = (float) (2 * PI / m_ring_segments);
//
//	const GLfloat tube_zero_pos = (float) glm::radians(0.0f);
//
//	std::vector<std::vector<glm::vec3>> vert_pos(ring_steps, std::vector<glm::vec3>(tube_steps));
//	std::vector<glm::vec3> centers;
//
//	//calculate coords of a ring
//	for (int i = 0; i < tube_steps; i++)
//	{
//		const GLfloat angle = tube_zero_pos + i * tube_delta_angle;
//		const GLfloat x = m_tube_radius + m_ring_radius * glm::cos(angle);
//		const GLfloat y = m_ring_radius * glm::sin(angle);
//		vert_pos[0][i] = glm::vec3(x, y, 0);
//	}
//
//
//	//calculate points for other ring segments
//	for (int i = 0; i < ring_steps; i++)
//	{
//		const float angle = i * ring_delta_angle;
//		const float sina = (float) glm::sin(angle);
//		const float cosa = (float)glm::cos(angle);
//
//		const glm::vec3 center(m_tube_radius * glm::cos(angle), 0, m_tube_radius * glm::sin(angle));
//		centers.push_back(center);
//
//		for (int j = 0; j < tube_steps; j++)
//		{
//			const glm::vec3 position = vert_pos[0][j];
//			const GLfloat x = position.x * cosa;
//			const GLfloat y = position.y;
//			const GLfloat z = position.x * sina;
//			vert_pos[i][j] = glm::vec3(x, y, z);
//		}
//	}
//
//	
//	GLfloat delta_u = m_ring_repeats / m_ring_segments;
//	GLfloat delta_v = m_tube_repeats / m_tube_segments;
//
//	for (int i = 0; i < tube_steps; i++)
//	{
//		for (int j = 0; j < ring_steps; j++)
//		{
//			m_texture_cords.push_back(j * delta_u);
//			m_texture_cords.push_back(i * delta_v);
//		}
//	}
//
//	//vec3 to GLfloat array
//	for (int i = 0; i < tube_steps; i++)
//	{
//		for (int j = 0; j < ring_steps; j++)
//		{
//			m_vertex_positions.push_back(vert_pos[j][i].x);
//			m_vertex_positions.push_back(vert_pos[j][i].y);
//			m_vertex_positions.push_back(vert_pos[j][i].z);
//
//			m_vertex_colours.push_back(0);
//			m_vertex_colours.push_back(0);
//			m_vertex_colours.push_back(0);
//			m_vertex_colours.push_back(1);
//
//			//calculate the normals
//			glm::vec3 normal = vert_pos[j][i] - centers[j];
//			m_normals.push_back(normal.x);
//			m_normals.push_back(normal.y);
//			m_normals.push_back(normal.z);
//		}
//	}
//
//
//	CreateBufferObjects();
//	
//	//calculate the faces
//	for (int i = 0; i < m_tube_segments; i++)
//	{
//		for (int j = 0; j < m_ring_segments; j++)
//		{
//			const int tl = j + i * ring_steps;
//			const int bl = j + (i + 1) * ring_steps;
//			const int br = j + 1 + (i + 1) * ring_steps;
//			const int tr = j + 1 + i * ring_steps;
//
//			//first tri
//			m_indices.push_back(tl);
//			m_indices.push_back(bl);
//			m_indices.push_back(tr);
//			//second tri
//			m_indices.push_back(bl);
//			m_indices.push_back(br);
//			m_indices.push_back(tr);
//		}
//	}
//
//
//	glGenBuffers(1, &m_element_buffer_object);
//	glBindBuffer(GL_ARRAY_BUFFER, m_element_buffer_object);
//	glBufferData(GL_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), m_indices.data(), GL_STATIC_DRAW);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//
//}
//
//void Torus::Draw()
//{
//	BindBuffers();
//
//	glPointSize(3.f);
//	if (m_draw_mode == MODE_WIREFRAME)
//		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	else
//		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//
//	if (m_draw_mode == MODE_POINTS)
//	{
//		glDrawArrays(GL_POINTS, 0, m_vertex_positions.size() / 3);
//	}
//	else
//	{
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_element_buffer_object);
//		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
//
//		glDrawElements(GL_TRIANGLE_STRIP, m_indices.size(), GL_UNSIGNED_INT, 0);
//	}
//	
//
//
//}
//
