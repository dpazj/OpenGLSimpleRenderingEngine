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
#include "OpenJoeL/Meshes/CubeMesh.h"
#include "OpenJoeL/Meshes/PlaneMesh.h"
#include "OpenJoeL/Meshes/ModelMesh.h"

#include "OpenJoeL/Meshes/Object.h"

#include "OpenJoeL/Texture/Texture.h"

#include "OpenJoeL/Utils/Camera.h"
#include "OpenJoeL/Utils/InputManager.h"
#include "OpenJoeL/Environment/Skybox.h"

#include "OpenJoeL/Render/DynamicCubemap.h"
#include "OpenJoeL/Lighting/Shadows.h"



// Include headers for our objects

GLint screen_width = 1680;
GLint screen_height = 1050;
GLfloat aspect_ratio = (GLfloat)screen_width / (GLfloat)screen_height;


//SHADERS
Shader* point_shadow_shader;
Shader* pbr_shader;
Shader* pbr_texture_shader;

//Objects

PBRObject* cube;

PBRTexturedObject* tabletop;
PBRTexturedObject* tablebot;
PBRTexturedObject* candle_holder1;
PBRTexturedObject* candle_holder2;



//OTHER
Camera camera(glm::vec3(0, 2, 5));
InputManager* input_manager;

PointShadowMap* point_shadow_map;

GLfloat delta_time = 0;
GLfloat last_frame = 0;

std::vector<glm::vec3> light_positions = {
	glm::vec3(0,6,0)
};

std::vector<glm::vec3> light_colours = {
	glm::vec3(1,1,1)
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

	input_manager->AddKey(GLFW_KEY_J, [glw]() {
		candle_holder1->transform.position.x += 0.001;
	});
	input_manager->AddKey(GLFW_KEY_L, [glw]() {
		candle_holder1->transform.position.x -= 0.001;
	});
	input_manager->AddKey(GLFW_KEY_I, [glw]() {
		candle_holder1->transform.position.y += 0.001;
	});
	input_manager->AddKey(GLFW_KEY_K, [glw]() {
		candle_holder1->transform.position.y -= 0.001;
	});

	input_manager->AddKey(GLFW_KEY_U, [glw]() {
		candle_holder1->transform.position.z += 0.001;
	});
	input_manager->AddKey(GLFW_KEY_O, [glw]() {
		candle_holder1->transform.position.z -= 0.001;
	});
}


void init(GLWrapper* glw)
{

	//load shaders
	try
	{

		pbr_shader = new Shader("../shaders/pbr.vert", "../shaders/pbr.frag");
		point_shadow_shader = new Shader("../shaders/pointshadow.vert", "../shaders/pointshadow.frag", "../shaders/pointshadow.gs");

		pbr_texture_shader = new Shader("../shaders/pbrtexture.vert", "../shaders/pbrtexture.frag");


	}
	catch (std::exception & e)
	{
		std::cout << "Caught exception: " << e.what() << std::endl;
		std::cin.ignore();
		exit(0);
	}

	setup_inputs(glw);

	PBRTextures tabletop_textures("../models/wooden-table/textures/topalbedo.png", "../models/wooden-table/textures/topnormal.png", "../models/wooden-table/textures/metallic.png", "../models/wooden-table/textures/topsmooth.png", "../models/wooden-table/textures/ao.png");
	PBRTextures tablebot_textures("../models/wooden-table/textures/botalbedo.png", "../models/wooden-table/textures/botnormal.png", "../models/wooden-table/textures/metallic.png", "../models/wooden-table/textures/botsmooth.png", "../models/wooden-table/textures/ao.png");

	ModelMesh tabletop_mesh;
	tabletop_mesh.SetMeshTextures(tabletop_textures);
	tabletop_mesh.LoadObject("../models/wooden-table/top.obj");
	tabletop = new PBRTexturedObject(tabletop_mesh.GetMesh());
	tabletop->transform.Scale(0.2f);

	ModelMesh tablebot_mesh;
	tablebot_mesh.SetMeshTextures(tablebot_textures);
	tablebot_mesh.LoadObject("../models/wooden-table/bot.obj");
	tablebot = new PBRTexturedObject(tablebot_mesh.GetMesh());
	tablebot->transform.Scale(0.2f);

	PBRTextures candle_textures("../models/candle-holder/textures/albedo.png", "../models/candle-holder/textures/normal.png", "../models/candle-holder/textures/metallic.png", "../models/candle-holder/textures/roughness.png", "../models/candle-holder/textures/ao.png");
	ModelMesh candle_mesh;
	candle_mesh.SetMeshTextures(candle_textures);
	candle_mesh.LoadObject("../models/candle-holder/candle.obj");
	candle_holder1 = new PBRTexturedObject(candle_mesh.GetMesh());
	candle_holder1->transform.Scale(2.0f);
	candle_holder1->transform.Rotate(90, Transform::Y);
	candle_holder1->transform.Translate(glm::vec3(-1.473f,5.596,-2.78));

	candle_holder2 = new PBRTexturedObject(candle_mesh.GetMesh());
	candle_holder2->transform.Scale(2.0f);
	candle_holder2->transform.Rotate(270, Transform::Y);
	candle_holder2->transform.Translate(glm::vec3(-1.473f, 5.596, 2.78));


	CubeMesh x; 
	x.Init();
	cube = new PBRObject(x);

}


void RenderPBRTexture(glm::mat4 projection, glm::mat4 view, glm::vec3 camera_pos, Shader * shader)
{
	shader->UseShader();
	shader->SetMat4("projection", projection);
	shader->SetMat4("view", view);
	shader->SetVec3("camera_position", camera_pos);

	tabletop->Draw(shader);
	tablebot->Draw(shader);

	candle_holder1->Draw(shader);
	candle_holder2->Draw(shader);

	std::cout << candle_holder1->transform.position.x << " " << candle_holder1->transform.position.y << " " << candle_holder1->transform.position.z <<  std::endl;
}

void RenderPBR(glm::mat4 projection, glm::mat4 view, glm::vec3 camera_pos, Shader* shader)
{
	shader->UseShader();
	shader->SetMat4("projection", projection);
	shader->SetMat4("view", view);
	shader->SetVec3("camera_position", camera_pos);

	//cube->Draw(shader);
}

void RenderScene(glm::mat4 projection, glm::mat4 view, glm::vec3 camera_pos)
{

	RenderPBRTexture(projection, view, camera_pos, pbr_texture_shader);
	RenderPBR(projection, view, camera_pos, pbr_shader);

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

	
	glViewport(0, 0, screen_width, screen_height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//PROPER DRAW
	glm::mat4 projection = glm::perspective(glm::radians(45.f), aspect_ratio, 0.1f, 100.0f);
	RenderScene(projection, camera.GetView(), camera.GetPosition());
	
	//LIGHTING
	
	for (int i = 0; i < light_positions.size(); i++)
	{
		pbr_texture_shader->UseShader();
		pbr_texture_shader->SetVec3(("light_positions[" + std::to_string(i) + "]").c_str(), light_positions.at(i));
		pbr_texture_shader->SetVec3(("light_colours[" + std::to_string(i) + "]").c_str(), light_colours.at(i));

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
