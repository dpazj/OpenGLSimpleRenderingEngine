//Joe Riemersma

#pragma once

#include "../Shaders/Shader.h"

#include <memory>
#include <vector>
#include <string>

#include <glload/gl_4_0.h>

#include <stb_image.h>

class Skybox
{
public:

	Skybox(std::vector<std::string> faces, std::shared_ptr<Shader> shader);

	void Init();
	void Draw();

	std::shared_ptr<Shader> GetShader();

private:

	std::shared_ptr<Shader> m_shader;

	std::vector<std::string> m_faces;
	GLuint m_skybox_vao = 0;
	GLuint m_skybox_vbo = 0;
	GLuint m_skybox_texture = 0;


	void LoadCubeMap();
};