//Joe Riemersma

#pragma once

#include <glm/glm.hpp>
#include <glload/gl_4_0.h>
#include <glload/gl_load.h>

#include <string>

class Shader{

public:
	Shader(const char* vertex_path, const char* fragment_path);
	void UseShader();

	void SetInt(const char* name, GLint value);
	void SetUInt(const char* name, GLuint value);
	void SetFloat(const char* name, GLfloat value);

	void SetVec3(const char* name,  glm::vec3 value);
	void SetVec4(const char* name, glm::vec4 value);

	void SetMat4(const char* name, glm::mat4 value);

private: 

	void LoadShader(const char* vertex_path, const char* fragment_path);
	GLuint BuildShader(GLenum eShaderType, const std::string& shaderText);
	GLuint BuildShaderProgram(std::string vertShaderStr, std::string fragShaderStr);
	std::string ReadFile(const char* filePath);

	GLuint m_program;

	

};