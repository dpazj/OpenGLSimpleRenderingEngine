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
		m_draw = false;
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

	void RenderShadowMap(std::function<void(glm::mat4, glm::mat4, glm::vec3)> render_function, Shader * shader)
	{
		if (m_type == Point)
		{
			m_point->RenderShadowMap(transform.position, render_function, shader);
		}
		else if (m_type == Directional)
		{
			
		}
	}


	void BindShadowMap(GLuint index)
	{
		if (m_type == Point)
		{
			m_point->BindShadowMap(index);
		}
		else if (m_type == Directional)
		{

		}
	}

	GLfloat GetFarPlane()
	{
		if (m_type == Point)
		{
			return m_point->GetFarPlane();
		}
		else if (m_type == Directional)
		{
			return m_directional->GetFarPlane();
		}

		return 0.0f;
	}

	
	void SetColour(glm::vec3 colour) { m_colour = colour; }
	void SetPower(GLfloat power) { m_power = power; }
	void SetShadowStrength(GLfloat power) { m_shadow_strength = power; }
	void SetShadowBias(GLfloat power) { m_shadow_bias = power; }


	glm::vec3 GetColour() { return m_colour; }
	GLfloat GetPower() { return m_power; }
	GLfloat GetShadowStrength() { return m_shadow_strength; }
	GLfloat GetShadowBias() { return m_shadow_bias; }



	LightType GetType() { return m_type; }

private:
	LightType m_type;

	DirectionalShadowMap * m_directional = 0;
	PointShadowMap * m_point = 0;


	glm::vec3 m_colour;
	GLfloat m_power = 10.0f;
	GLfloat m_shadow_strength = 1.0f;
	GLfloat m_shadow_bias = 0.25f;
};