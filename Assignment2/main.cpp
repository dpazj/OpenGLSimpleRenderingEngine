/*
//Joe Riemersma
*/


/* Link to static libraries, could define these as linker inputs in the project settings instead
if you prefer */
#ifdef _DEBUG
#pragma comment(lib, "glfw3D.lib")
#pragma comment(lib, "glloadD.lib")
#pragma comment(lib, "assimp-vc140-mt.lib")
#else
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glload.lib")
#pragma comment(lib, "assimp-vc140-mt.lib")
#endif
#pragma comment(lib, "opengl32.lib")

#define STB_IMAGE_IMPLEMENTATION

/* Include the header to the GLFW wrapper class which
also includes the OpenGL extension initialisation*/
#include "wrapper_glfw.h"
#include <iostream>

// We'll use the STD stack class to make our stack or matrices
#include <stack>
#include <memory>
#include <vector>
#include <string>

/* Include GLM core and matrix extensions*/
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>


#include "OpenJoeL/Shaders/Shader.h"
#include "OpenJoeL/Meshes/ModelMesh.h"
#include "OpenJoeL/Meshes/SphereMesh.h"

#include "OpenJoeL/Texture/Texture.h"

#include "OpenJoeL/Utils/Camera.h"
#include "OpenJoeL/Utils/InputManager.h"

// Include headers for our objects

GLint screen_width = 1680;
GLint screen_height = 1050;
GLfloat aspect_ratio = (GLfloat)screen_width / (GLfloat)screen_height;




//SHADERS
//std::shared_ptr<Shader> shader;
Shader* shader;


ModelMesh* test;

Texture* test_tex;
unsigned int albedo, normal, metallic, roughness, ao;



Camera camera(glm::vec3(0, 0, 5));

InputManager* input_manager;


GLfloat delta_time = 0;
GLfloat last_frame = 0;


glm::vec3 lightPositions[] = {
	glm::vec3(-10.0f,  10.0f, 10.0f),
	glm::vec3(10.0f,  10.0f, 10.0f),
	glm::vec3(-10.0f, -10.0f, 10.0f),
	glm::vec3(10.0f, -10.0f, 10.0f),
};
glm::vec3 lightColors[] = {
	glm::vec3(300.0f, 300.0f, 300.0f),
	glm::vec3(300.0f, 300.0f, 300.0f),
	glm::vec3(300.0f, 300.0f, 300.0f),
	glm::vec3(300.0f, 300.0f, 300.0f)
};

unsigned int loadTexture(char const* path);


void setup_inputs(GLWrapper* glw)
{
	//Camera controls
	input_manager->AddKey(GLFW_KEY_W, []() {
		camera.ProcessKeyboard(FORWARD, delta_time);
	});

	input_manager->AddKey(GLFW_KEY_S, []() {
		camera.ProcessKeyboard(BACK, delta_time);
	});

	input_manager->AddKey(GLFW_KEY_A, []() {
		camera.ProcessKeyboard(LEFT, delta_time);
	});

	input_manager->AddKey(GLFW_KEY_D, []() {
		camera.ProcessKeyboard(RIGHT, delta_time);
	});

	input_manager->SetMouseCallback([](GLFWwindow* window, double x, double y) {
		camera.ProcessMouse((GLfloat)x, (GLfloat)y);
	});

	input_manager->AddKey(GLFW_KEY_ESCAPE, [glw]() {
		glfwSetWindowShouldClose(glw->getWindow(), GL_TRUE);
	});

}


void init(GLWrapper* glw)
{
	

	input_manager->ProcessInput();

	//load shaders
	try
	{
		shader = new Shader("../shaders/pbrtexture.vert", "../shaders/pbrtexture.frag");
		//shader = std::make_shared<Shader>("pbr.vert", "pbr.frag");

	}
	catch (std::exception & e)
	{
		std::cout << "Caught exception: " << e.what() << std::endl;
		std::cin.ignore();
		exit(0);
	}

	setup_inputs(glw);
	shader->UseShader();

	PBRTextures textures("../models/Cerberus/Textures/Cerberus_A.tga", "../models/Cerberus/Textures/Cerberus_N.tga", "../models/Cerberus/Textures/Cerberus_M.tga", "../models/Cerberus/Textures/Cerberus_R.tga", "../models/Cerberus/Textures/Cerberus_AO.tga");

	test = new ModelMesh();
	test->SetMeshTextures(textures);
	////test->LoadObject("export3dcoat.obj");
	test->LoadObject("../models/Cerberus/Cerberus_LP.obj");
}




void display()
{
	const GLfloat currentFrame = (GLfloat)glfwGetTime();
	delta_time = currentFrame - last_frame;
	last_frame = currentFrame;

	input_manager->ProcessInput();

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.859375f, 0.859375f, 0.859375f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	const glm::mat4 projection = glm::perspective(glm::radians(30.0f), aspect_ratio, 0.1f, 100.0f);
	const glm::mat4 view = camera.GetView();


	shader->UseShader();
	shader->SetMat4("view", view);
	shader->SetMat4("projection", projection);
	shader->SetVec3("camera_position", camera.GetPosition());

	glm::mat4 model(1.0f);
	shader->SetMat4("model", model);
	test->Draw(shader);

	model = glm::scale(model,glm::vec3(0.5f,0.5f,0.5f));
	model = glm::translate(model, glm::vec3(0.2f, 0.7f, 0.0f));
	shader->SetMat4("model", model);
	test->Draw(shader);

	for (unsigned int i = 0; i < 4; ++i)
	{
		glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
		newPos = lightPositions[i];
		shader->SetVec3(("light_positions[" + std::to_string(i) + "]").c_str(), newPos);
		shader->SetVec3(("light_colors[" + std::to_string(i) + "]").c_str(), lightColors[i]);
	}

}

void modifyTexture(GLuint texture)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}



/* Called whenever the window is resized. The new window size is given, in pixels. */
static void reshape(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	// Store aspect ratio to use for our perspective projection
	aspect_ratio = float(w) / float(h);
}

unsigned int loadTexture(char const* path)
{
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


/* Entry point of program */
int main(int argc, char* argv[])
{
	GLWrapper* glw = new GLWrapper(screen_width, screen_height, "PBR");
	input_manager = new InputManager(glw->getWindow());

	if (!ogl_LoadFunctions())
	{
		fprintf(stderr, "ogl_LoadFunctions() failed. Exiting\n");
		return 0;
	}

	// Register the callback functions
	glw->setRenderer(display);
	glw->setReshapeCallback(reshape);

	/* Output the OpenGL vendor and version */
	glw->DisplayVersion();

	init(glw);

	glw->eventLoop();

	delete(glw);
	return 0;
}
