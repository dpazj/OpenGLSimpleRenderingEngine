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
#include <thread>


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

const GLfloat REFLECTION_RESOLUTION = 1024;
GLint screen_width = 1680;
GLint screen_height = 1050;
GLfloat aspect_ratio = (GLfloat)screen_width / (GLfloat)screen_height;


//SHADERS
Shader* point_shadow_shader;
Shader* pbr_shader;
Shader* pbr_texture_shader;
Shader* pbr_texture_reflection_shader;

//Objects
PBRTexturedObject* floorplane;
PBRTexturedObject* wallplane;

PBRTexturedObject* tabletop;
PBRTexturedObject* tablebot;

PBRObject* window;

PBRTexturedReflectObject* candle_holder1;
PBRTexturedReflectObject* candle_holder2;

std::vector<PBRTexturedReflectObject*> texture_reflect_objects;

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
void RenderScene(glm::mat4 projection, glm::mat4 view, glm::vec3 camera_pos);

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

	input_manager->AddKey(GLFW_KEY_N, [glw]() {
		candle_holder1->transform.Rotate(-1, Transform::Y);
	});
	input_manager->AddKey(GLFW_KEY_M, [glw]() {
		candle_holder1->transform.Rotate(1, Transform::Y);
	});
}


void CreateTable()
{
	PBRTextures tabletop_textures("../models/wooden-table/textures/topalbedo.png", "../models/wooden-table/textures/topnormal.png", "../models/wooden-table/textures/metallic.png", "../models/wooden-table/textures/topsmooth.png", "../models/wooden-table/textures/ao.png");
	PBRTextures tablebot_textures("../models/wooden-table/textures/botalbedo.png", "../models/wooden-table/textures/botnormal.png", "../models/wooden-table/textures/metallic.png", "../models/wooden-table/textures/botsmooth.png", "../models/wooden-table/textures/ao.png");
	
	ModelMesh tabletop_mesh;
	tabletop_mesh.SetMeshTextures(tabletop_textures);
	tabletop_mesh.LoadObject("../models/wooden-table/top.obj");

	ModelMesh tablebot_mesh;
	tablebot_mesh.SetMeshTextures(tablebot_textures);
	tablebot_mesh.LoadObject("../models/wooden-table/bot.obj");

	tabletop = new PBRTexturedObject(tabletop_mesh.GetMesh());
	tabletop->transform.Scale(0.2f);
	tablebot = new PBRTexturedObject(tablebot_mesh.GetMesh());
	tablebot->transform.Scale(0.2f);
}


void init(GLWrapper* glw)
{

	//load shaders
	try
	{

		pbr_shader = new Shader("../shaders/pbr.vert", "../shaders/pbr.frag");
		point_shadow_shader = new Shader("../shaders/pointshadow.vert", "../shaders/pointshadow.frag", "../shaders/pointshadow.gs");

		pbr_texture_shader = new Shader("../shaders/pbrtexture.vert", "../shaders/pbrtexture.frag");
		pbr_texture_reflection_shader = new Shader("../shaders/pbrtexturereflection.vert", "../shaders/pbrtexturereflection.frag");

	}
	catch (std::exception & e)
	{
		std::cout << "Caught exception: " << e.what() << std::endl;
		std::cin.ignore();
		exit(0);
	}

	setup_inputs(glw);

	GLfloat start = glfwGetTime();
	std::cout << "Loading objects..." << std::endl;

	//LOAD TEXTURES
	
	//PBRTextures candle_textures("../models/candle-holder/textures/albedo.png", "../models/candle-holder/textures/normal.png", "../models/candle-holder/textures/metallic.png", "../models/candle-holder/textures/roughness.png", "../models/candle-holder/textures/ao.png");
	PBRTextures floor_textures("../models/floor/albedo.png", "../models/floor/normal.png", "../models/floor/metallic.psd", "../models/floor/roughness.png", "../models/floor/ao.png");
	PBRTextures wall_textures("../models/wall/albedo.png", "../models/wall/normal.png", "../models/wall/metallic.png", "../models/wall/roughness.png", "../models/wall/ao.png");


	//LOAD MESHES
	ModelMesh window_mesh;
	window_mesh.LoadObject("../models/window/window.obj");
	//window_mesh.LoadObject("../models/candle-holder/candle.obj");

	window = new PBRObject(window_mesh.GetMesh());
	window->SetPBRProperties(glm::vec3(144.0f/255.0f,89.0f/255.0f, 35.0f/255.0f), 0.2f, 0.3f); //
	window->transform.Scale(2.0f);
	window->transform.scale.y + 0.1f;


	/*ModelMesh candle_mesh;
	candle_mesh.SetMeshTextures(candle_textures);
	candle_mesh.LoadObject("../models/candle-holder/candle.obj");*/

	//CreateTable();

	PlaneMesh floor_mesh;
	floor_mesh.Init();
	floor_mesh.SetMeshTextures(floor_textures);

	GLfloat end = glfwGetTime();
	std::cout << "Load objects in: " << (end - start) << " seconds" << std::endl;

	//CREATE OBJECTS
	


	/*candle_holder1 = new PBRTexturedReflectObject(candle_mesh.GetMesh());
	candle_holder1->CreateDynamicCubeMap(REFLECTION_RESOLUTION);
	candle_holder1->transform.Scale(2.0f);
	candle_holder1->transform.Rotate(90, Transform::Y);
	candle_holder1->transform.Translate(glm::vec3(-1.473f,5.596,-2.78));

	candle_holder2 = new PBRTexturedReflectObject(candle_mesh.GetMesh());
	candle_holder2->CreateDynamicCubeMap(REFLECTION_RESOLUTION);
	candle_holder2->transform.Scale(2.0f);
	candle_holder2->transform.Rotate(270, Transform::Y);
	candle_holder2->transform.Translate(glm::vec3(-1.473f, 5.596, 2.78));*/

	floorplane = new PBRTexturedObject(floor_mesh.GetMesh());
	floorplane->transform.Scale(glm::vec3(10,1,10));

	floor_mesh.SetMeshTextures(wall_textures);
	wallplane = new PBRTexturedObject(floor_mesh.GetMesh());
	wallplane->transform.Scale(glm::vec3(4, 1, 4));
	wallplane->transform.Rotate(90, Transform::Z);


	//Init lists
	/*texture_reflect_objects.push_back(candle_holder1);
	texture_reflect_objects.push_back(candle_holder2);*/

	//configure shaders
	pbr_texture_reflection_shader->UseShader();
	pbr_texture_reflection_shader->SetInt("reflection_cube", 10);

}

void RenderTextureReflections() 
{
	for (auto obj : texture_reflect_objects)
	{
		obj->Hide(true);
		glm::vec3 pos = obj->transform.position;
		obj->RenderCubemap([pos](glm::mat4 projection, glm::mat4 view) {RenderScene(projection, view, pos);});
		obj->Hide(false);
	}
}

void RenderReflections() {
	RenderTextureReflections();
}


void RenderPBRTextureReflect(glm::mat4 projection, glm::mat4 view, glm::vec3 camera_pos, Shader* shader)
{
	shader->UseShader();
	shader->SetMat4("projection", projection);
	shader->SetMat4("view", view);
	shader->SetVec3("camera_position", camera_pos);

	//candle_holder1->Draw(shader);
	//candle_holder2->Draw(shader);

}

void RenderPBRTexture(glm::mat4 projection, glm::mat4 view, glm::vec3 camera_pos, Shader * shader)
{
	shader->UseShader();
	shader->SetMat4("projection", projection);
	shader->SetMat4("view", view);
	shader->SetVec3("camera_position", camera_pos);

	/*tabletop->Draw(shader);
	tablebot->Draw(shader);*/


	//floor
	for (float i = 0.0f; i < 2.0f; i++)
	{
		floorplane->transform.position = glm::vec3(-10.0f, i * 23.0f, 20.0f);
		floorplane->Draw(shader);
		floorplane->transform.position = glm::vec3(-10.0f, i * 23.0f, 0.0f);
		floorplane->Draw(shader);
		floorplane->transform.position = glm::vec3(-10.0f, i * 23.0f, -20.0f);
		floorplane->Draw(shader);
		floorplane->transform.position = glm::vec3(10.0f, i * 23.0f, 20.0f);
		floorplane->Draw(shader);
		floorplane->transform.position = glm::vec3(10.0f, i * 23.0f, 0.0f);
		floorplane->Draw(shader);
		floorplane->transform.position = glm::vec3(10.0f, i * 23.0f, -20.0f);
		floorplane->Draw(shader);
		floorplane->transform.Rotate(180, Transform::X);
	}

	//walls

	//two sides
	auto start = glm::vec3(0, 4, 26);
	wallplane->transform.y_angle = 0;
	for (int x = -1; x < 2; x += 2) {
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				wallplane->transform.position = start + glm::vec3(x * 20, j * 8, i * -8);
				wallplane->Draw(shader);
			}
		}
	}

	wallplane->transform.y_angle = 90;
	start = glm::vec3(16, 4, -30);
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			wallplane->transform.position = start + glm::vec3(-8 * i, j*8, 0);
			wallplane->Draw(shader);
		}
		
	}
	
	start = glm::vec3(16, 4, 30);
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			wallplane->transform.position = start + glm::vec3(-8 * i, j * 19, 0);
			wallplane->Draw(shader);
		}
	}

	glm::vec3 tmp_scale = wallplane->transform.scale;


	start = glm::vec3(0, 9.9, 30);
	wallplane->transform.scale = glm::vec3(1.9, 1, 3);

	
	for (int i = 0; i < 3; i++)
	{
		for (int j = -1; j < 2; j+=2){
			wallplane->transform.position = start + glm::vec3(j * -18, 3.8 * i,0);
			wallplane->Draw(shader);
		}

		for (int j = -1; j < 2; j+=2) {
			wallplane->transform.position = start + glm::vec3(j * -6, 3.8 * i, 0);
			wallplane->Draw(shader);
		}
	}
	wallplane->transform.scale = tmp_scale;
}

void RenderPBR(glm::mat4 projection, glm::mat4 view, glm::vec3 camera_pos, Shader* shader)
{
	shader->UseShader();
	shader->SetMat4("projection", projection);
	shader->SetMat4("view", view);
	shader->SetVec3("camera_position", camera_pos);

	const auto start = glm::vec3(0, 13.7, 30);

	for (int i = -1; i < 2; i++)
	{
		window->transform.position = start + glm::vec3(-12 * i, 0, 0);
		window->Draw(shader);
	}
	
	//cube->Draw(shader);
}

void RenderScene(glm::mat4 projection, glm::mat4 view, glm::vec3 camera_pos)
{
	RenderPBRTextureReflect(projection, view, camera_pos, pbr_texture_reflection_shader);
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
	//glClearColor(0.859375f, 0.859375f, 0.859375f, 1.0f);
	glClearColor(0, 0, 0, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	


	//RENDER REFLECTIONS;
	RenderReflections();



	glViewport(0, 0, screen_width, screen_height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//PROPER DRAW
	glm::mat4 projection = glm::perspective(glm::radians(80.f), aspect_ratio, 0.1f, 300.0f);
	RenderScene(projection, camera.GetView(), camera.GetPosition());
	
	//LIGHTING
	
	for (int i = 0; i < light_positions.size(); i++)
	{
		pbr_texture_shader->UseShader();
		pbr_texture_shader->SetVec3(("light_positions[" + std::to_string(i) + "]").c_str(), light_positions.at(i));
		pbr_texture_shader->SetVec3(("light_colours[" + std::to_string(i) + "]").c_str(), light_colours.at(i));

		pbr_shader->UseShader();
		pbr_shader->SetVec3(("light_positions[" + std::to_string(i) + "]").c_str(), light_positions.at(i));
		pbr_shader->SetVec3(("light_colours[" + std::to_string(i) + "]").c_str(), light_colours.at(i));

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
