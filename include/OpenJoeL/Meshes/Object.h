#pragma once


#include "OpenJoeL/Meshes/Mesh.h"
#include "OpenJoeL/Shaders/Shader.h"
#include "OpenJoeL/Render/DynamicCubemap.h"


#include "glm/gtc/matrix_transform.hpp"

#include <memory>


class Transform
{
public:

	Transform()
	{
		position = glm::vec3(0, 0, 0);
		scale = glm::vec3(1, 1, 1);
	}
	enum Axis { X, Y, Z };

	glm::mat4 GetModel(glm::mat4 i)
	{
		
		i = glm::rotate(i, x_angle, glm::vec3(1, 0, 0));
		i = glm::rotate(i, y_angle, glm::vec3(0, 1, 0));
		i = glm::rotate(i, z_angle, glm::vec3(0, 0, 1));
		i = glm::translate(i, position);
		i = glm::scale(i, scale);

		return i;
	}

	void Scale(GLfloat new_scale)
	{
		scale *= new_scale;
	}

	void Scale(glm::vec3 new_scale)
	{
		scale = new_scale;
	}

	void Translate(glm::vec3 new_translation)
	{
		position = new_translation;
	}

	void Rotate(GLfloat angle, Axis axis)
	{
		switch (axis)
		{
		case Transform::X:
			x_angle = angle;
			break;
		case Transform::Y:
			y_angle = angle;
			break;
		case Transform::Z:
			z_angle = angle;
			break;
		default:
			break;
		}
	}

	glm::vec3 position;
	glm::vec3 scale;

	GLfloat x_angle = 0;
	GLfloat y_angle = 0;
	GLfloat z_angle = 0;

};


class Object
{
public:

	
	Object(Mesh mesh) {
		m_mesh = mesh;
	};
	virtual ~Object() = default;

	virtual void Draw(Shader* shader, glm::mat4 idendity = glm::mat4(1.0f)) {};
	Transform transform;
protected:
	Mesh m_mesh;
};

class PBRObject : public Object
{
public:
	PBRObject(Mesh mesh) : Object(mesh)
	{
	};

	void Draw(Shader* shader, glm::mat4 idendity = glm::mat4(1.0f)) override
	{
		shader->SetFloat("metallic", m_metallic);
		shader->SetFloat("roughness", m_roughness);
		shader->SetFloat("ambient_occlusion", m_ambient_occlusion);
		shader->SetMat4("model", transform.GetModel(idendity));
		shader->SetVec3("albedo", m_albedo);

		m_mesh.Draw(shader);
	};

	void SetPBRProperties(glm::vec3 albedo, GLfloat metallic = 1.0f, GLfloat roughness = 0.05f, GLfloat ambient_occlusion = 1.0f)
	{
		m_albedo = albedo;
		m_metallic = metallic;
		m_roughness = roughness;
		m_ambient_occlusion = ambient_occlusion;
	};

private:
	glm::vec3 m_albedo = glm::vec3(0, 0, 0);
	GLfloat m_metallic = 1.0f;
	GLfloat m_roughness = 0.05f;
	GLfloat m_ambient_occlusion = 1.0f;
};


class PBRReflectObject : public Object
{
public:
	PBRReflectObject(Mesh mesh) : Object(mesh)
	{
	};

	void Draw(Shader* shader, glm::mat4 idendity = glm::mat4(1.0f)) override
	{
		m_reflection_cubemap->BindCubemap();
		shader->SetFloat("metallic", m_metallic);
		shader->SetFloat("roughness", m_roughness);
		shader->SetFloat("ambient_occlusion", m_ambient_occlusion);
		shader->SetMat4("model", transform.GetModel(idendity));
		shader->SetVec3("albedo", m_albedo);

		m_mesh.Draw(shader);
	};

	void CreateDynamicCubeMap(GLfloat cube_map_resolution)
	{
		m_reflection_cubemap = new DynamicCubemap(cube_map_resolution);
	};

	void RenderCubemap(std::function<void(glm::mat4, glm::mat4)> render_function)
	{
		m_reflection_cubemap->RenderCubemap(transform.position, render_function);
	}

	void SetPBRProperties(glm::vec3 albedo, GLfloat metallic = 1.0f, GLfloat roughness = 0.2f, GLfloat ambient_occlusion = 1.0f)
	{
		m_albedo = albedo;
		m_metallic = metallic;
		m_roughness = roughness;
		m_ambient_occlusion = ambient_occlusion;
	};

private:
	DynamicCubemap* m_reflection_cubemap;

	glm::vec3 m_albedo = glm::vec3(0, 0, 0);
	GLfloat m_metallic = 1.0f;
	GLfloat m_roughness = 0.05f;
	GLfloat m_ambient_occlusion = 1.0f;
};


