#pragma once

#include "../Shaders/Shader.h"
#include "stb_image.h"
#include <glload/gl_4_0.h>
#include <vector>
#include <string>
#include <iostream>

class Texture
{
public:
	Texture(const std::string& name, const std::string& path)
	{
		m_name = name;
		m_id = LoadTexture(path.c_str());
		m_path = path;
	}
	GLuint GetID() { return m_id; }
	std::string GetName() { return m_name; }
	std::string GetPath() { return m_path; }



private:
	std::string m_name;
	std::string m_path;
	GLuint m_id;

	unsigned int LoadTexture(char const* path)
	{
		stbi_set_flip_vertically_on_load(true);
		unsigned int textureID;
		glGenTextures(1, &textureID);
		
		int width, height, nrComponents;
		unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format = 0;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;
		
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}
		
		return textureID;
	}
};

class MeshTextures
{
public:
	void SetMeshTextures(std::vector<Texture> textures)
	{
		m_textures = textures;
	}
	void AddTexture(Texture texture)
	{
		m_textures.push_back(texture);
	}
	void BindTexures(Shader* shader)
	{
		shader->UseShader();
		for (GLuint i = 0; i < m_textures.size(); i++)
		{
			
			shader->SetInt(m_textures[i].GetName().c_str(), i);
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, m_textures[i].GetID());
		}
		glActiveTexture(GL_TEXTURE0);
	}
	
private:
	std::vector<Texture> m_textures;
};


class PBRTextures : public MeshTextures
{
public:
	PBRTextures(const std::string& albedo_map, const std::string& normal_map, const std::string& metallic_map, const std::string roughness_map, const std::string ao_map)
	{
		AddTexture(Texture("albedo_map", albedo_map));
		AddTexture(Texture("normal_map", normal_map));
		AddTexture(Texture("metallic_map", metallic_map));
		AddTexture(Texture("roughness_map", roughness_map));
		AddTexture(Texture("ambient_occlusion_map", ao_map));
	}
};

