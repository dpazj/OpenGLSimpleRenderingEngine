////Joe Riemersma
#pragma once

#include "OpenJoeL/Meshes/Object.h"
#include "OpenJoeL/Meshes/Mesh.h"
#include "OpenJoeL/Shaders/Shader.h"
#include "Shadows.h"


class LightSource : public Object
{
public:
	enum LightType{
		Point, Directional
	};

	LightSource(Mesh mesh, LightType type) : Object(mesh)
	{
		m_colour = glm::vec3(1);
		m_type = type;
		//m_draw = false;
	}

	void InitShadowMap(GLfloat size)
	{
		if (m_type == Point)
		{
			m_point = new PointShadowMap(size);
		}
		else if(m_type == Directional)
		{
			m_directional = new DirectionalShadowMap(size);
		}
	}

	void RenderShadowMap(std::function<void(glm::mat4, glm::mat4)> render_function, Shader * shader)
	{
		if (m_type == Point)
		{
			m_point->RenderShadowMap(transform.position, render_function, shader);
		}
		else if (m_type == Directional)
		{
			
		}
	}

	void SetColour(glm::vec3 colour) { m_colour = colour; }
	glm::vec3 GetColour() {return m_colour; }


private:
	LightType m_type;

	DirectionalShadowMap * m_directional = 0;
	PointShadowMap * m_point = 0;

	glm::vec3 m_colour;

};