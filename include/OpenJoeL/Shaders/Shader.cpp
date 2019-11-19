//Joe Riemersma

#include "Shader.h"
#include <iostream>
#include <fstream>
#include <vector>

Shader::Shader(const char* vertex_path, const char* fragment_path)
{
	LoadShader(vertex_path, fragment_path);
}

void Shader::UseShader()
{
	glUseProgram(m_program);
}

void Shader::SetUInt(const char* name, GLuint value)
{
	glUniform1ui(glGetUniformLocation(m_program, name), value);
}

void Shader::SetInt(const char* name, GLint value)
{
	glUniform1i(glGetUniformLocation(m_program, name), value);
}

void Shader::SetFloat(const char* name, GLfloat value)
{
	glUniform1f(glGetUniformLocation(m_program, name), value);
}

void Shader::SetVec3(const char* name, glm::vec3 value)
{
	glUniform3fv(glGetUniformLocation(m_program, name),1,&value[0]);
}

void Shader::SetVec4(const char* name, glm::vec4 value)
{
	glUniform4fv(glGetUniformLocation(m_program, name), 1, &value[0]);
}

void Shader::SetMat4(const char* name, glm::mat4 value)
{
	glUniformMatrix4fv(glGetUniformLocation(m_program, name), 1, GL_FALSE, &value[0][0]);
}


GLuint Shader::BuildShader(GLenum eShaderType, const std::string& shaderText)
{
	GLuint shader = glCreateShader(eShaderType);
	const char* strFileData = shaderText.c_str();
	glShaderSource(shader, 1, &strFileData, NULL);

	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		// Output the compile errors

		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar* strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

		const char* strShaderType = NULL;
		switch (eShaderType)
		{
		case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
		case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
		case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
		}

		std::cerr << "Compile error in " << strShaderType << "\n\t" << strInfoLog << std::endl;
		delete[] strInfoLog;

		throw std::exception("Shader compile exception");
	}

	return shader;
}
std::string Shader::ReadFile(const char* filePath)
{
	std::string content;
	std::ifstream fileStream(filePath, std::ios::in);

	if (!fileStream.is_open()) {
		std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
		return "";
	}

	std::string line = "";
	while (!fileStream.eof()) {
		getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}
void Shader::LoadShader(const char* vertex_path, const char* fragment_path)
{
	GLuint vertShader, fragShader;

	// Read shaders
	std::string vertShaderStr = ReadFile(vertex_path);
	std::string fragShaderStr = ReadFile(fragment_path);

	GLint result = GL_FALSE;
	int logLength;

	vertShader = BuildShader(GL_VERTEX_SHADER, vertShaderStr);
	fragShader = BuildShader(GL_FRAGMENT_SHADER, fragShaderStr);

	GLuint program = glCreateProgram();
	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &result);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	std::vector<char> programError((logLength > 1) ? logLength : 1);
	glGetProgramInfoLog(program, logLength, NULL, &programError[0]);
	std::cout << &programError[0] << std::endl;

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	m_program = program;
}
GLuint Shader::BuildShaderProgram(std::string vert_shader_str, std::string frag_shader_Str)
{
	GLuint vertShader, fragShader;
	GLint result = GL_FALSE;

	try
	{
		vertShader = BuildShader(GL_VERTEX_SHADER, vert_shader_str);
		fragShader = BuildShader(GL_FRAGMENT_SHADER, frag_shader_Str);
	}
	catch (std::exception & e)
	{
		std::cout << "Exception: " << e.what() << std::endl;
		throw std::exception("BuildShaderProgram() Build shader failure. Abandoning");
	}

	GLuint program = glCreateProgram();
	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);

	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{

		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar* strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		std::cerr << "Linker error: " << strInfoLog << std::endl;

		delete[] strInfoLog;
		throw std::runtime_error("Shader could not be linked.");
	}

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	return program;
}

