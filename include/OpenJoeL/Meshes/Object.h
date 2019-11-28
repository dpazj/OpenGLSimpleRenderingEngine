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
		i = glm::translate(i, position);
	
		i = glm::rotate(i, glm::radians(x_angle), glm::vec3(1, 0, 0));
		i = glm::rotate(i, glm::radians(y_angle), glm::vec3(0, 1, 0));
		i = glm::rotate(i, glm::radians(z_angle), glm::vec3(0, 0, 1));
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
			x_angle += angle;
			break;
		case Transform::Y:
			y_angle += angle;
			break;
		case Transform::Z:
			z_angle += angle;
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

	virtual void Draw(Shader* shader, glm::mat4 idendity = glm::mat4(1.0f)) {
		if (!m_draw) { return; }

		shader->SetMat4("model", transform.GetModel(idendity));
		m_mesh.Draw(shader);
	};

	Transform transform;

	void Hide(bool hide)
	{
		m_draw = !hide;
	}

protected:
	Mesh m_mesh;
	bool m_draw = true;
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



class PBRTexturedObject : public Object
{
public:
	PBRTexturedObject(Mesh mesh) : Object(mesh)
	{
	};

	void Draw(Shader* shader, glm::mat4 idendity = glm::mat4(1.0f)) override
	{
		if (!m_draw) { return; }

		shader->SetMat4("model", transform.GetModel(idendity));
		m_mesh.Draw(shader);
	};

};


class PBRTexturedReflectObject : public Object
{
public:
	PBRTexturedReflectObject(Mesh mesh) : Object(mesh)
	{
		m_reflection_cubemap = nullptr;
	};

	void Draw(Shader* shader, glm::mat4 idendity = glm::mat4(1.0f)) override
	{
		if (!m_draw) { return; }

		m_reflection_cubemap->BindCubemap();
		shader->SetMat4("model", transform.GetModel(idendity));
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

private:
	DynamicCubemap* m_reflection_cubemap;
};


class PBRReflectObject : public Object
{
public:
	PBRReflectObject(Mesh mesh) : Object(mesh)
	{
		m_reflection_cubemap = nullptr;
	};

	void Draw(Shader* shader, glm::mat4 idendity = glm::mat4(1.0f)) override
	{
		if (!m_draw) { return; }
		
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

	void SetPBRProperties(glm::vec3 albedo, GLfloat metallic = 1.0f, GLfloat roughness = 0.8f, GLfloat ambient_occlusion = 1.0f)
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


