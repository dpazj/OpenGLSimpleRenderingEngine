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
#include "OpenJoeL/Lighting/Lighting.h"
#include "OpenJoeL/Texture/Texture.h"

#include "OpenJoeL/Utils/Camera.h"
#include "OpenJoeL/Utils/InputManager.h"
#include "OpenJoeL/Environment/Skybox.h"

#include "OpenJoeL/Render/DynamicCubemap.h"
#include "OpenJoeL/Lighting/Shadows.h"



// Include headers for our objects

const GLfloat REFLECTION_RESOLUTION = 1024;
const GLfloat SHADOW_RESOLUTION = 1024;

GLint screen_width = 1680;
GLint screen_height = 1050;
GLfloat aspect_ratio = (GLfloat)screen_width / (GLfloat)screen_height;


//SHADERS
Shader* point_shadow_shader;
Shader* pbr_shader;
Shader* pbr_texture_shader;
Shader* pbr_texture_reflection_shader;
Shader* simple_shader;
Shader* skybox_shader;

//Objects
PBRTexturedObject* floorplane;
PBRTexturedObject* wallplane;

PBRTexturedObject* tabletop;
PBRTexturedObject* tablebot;
PBRTexturedObject* chandelier;

PBRTexturedObject* door;


PBRTexturedObject* chest;

PBRObject* window;

PBRTexturedReflectObject* candle_holder1;
PBRTexturedReflectObject* candle_holder2;



Skybox* skybox;


std::vector<PBRTexturedReflectObject*> texture_reflect_objects;

//Lighting
Lighting scene_lighting;
LightSource* moveable_light;


//OTHER
Camera camera(glm::vec3(0, 2, 5));
InputManager* input_manager;

GLfloat delta_time = 0;
GLfloat last_frame = 0;


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
		moveable_light->transform.position.x += 0.05;
	});
	input_manager->AddKey(GLFW_KEY_L, [glw]() {
		moveable_light->transform.position.x -= 0.05;
	});
	input_manager->AddKey(GLFW_KEY_I, [glw]() {
		moveable_light->transform.position.y += 0.05;
	});
	input_manager->AddKey(GLFW_KEY_K, [glw]() {
		moveable_light->transform.position.y -= 0.05;
	});

	input_manager->AddKey(GLFW_KEY_U, [glw]() {
		moveable_light->transform.position.z += 0.05;
	});
	input_manager->AddKey(GLFW_KEY_O, [glw]() {
		moveable_light->transform.position.z -= 0.05;
	});

	input_manager->AddKey(GLFW_KEY_N, [glw]() {
		moveable_light->transform.Rotate(-1, Transform::Y);
	});
	input_manager->AddKey(GLFW_KEY_M, [glw]() {
		moveable_light->transform.Rotate(1, Transform::Y);
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

		simple_shader = new Shader("../shaders/simple.vert", "../shaders/simple.frag");
		skybox_shader = new Shader("../shaders/skybox.vert", "../shaders/skybox.frag");
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
	PBRTextures chandelier_textures("../models/chandelier/textures/albedo.png", "../models/chandelier/textures/normal.png", "../models/chandelier/textures/metallic.png", "../models/chandelier/textures/roughness.png", "../models/chandelier/textures/ao.png");
	PBRTextures chest_textures("../models/chest/textures/albedo.jpeg", "../models/chest/textures/normal.jpeg", "../models/chest/textures/metallic.jpeg", "../models/chest/textures/roughness.jpeg", "../models/chest/textures/ao.jpeg");

	PBRTextures door_textures("../models/door/textures/albedo.png", "../models/door/textures/normal.png", "../models/door/textures/roughness.png", "../models/door/textures/roughness.png", "../models/door/textures/ao.png");
	


	//LOAD MESHES
	ModelMesh chandeliermesh; chandeliermesh.LoadObject("../models/chandelier/Chandelier.fbx"); chandeliermesh.SetMeshTextures(chandelier_textures);
	ModelMesh window_mesh; window_mesh.LoadObject("../models/window/window.obj");
	ModelMesh chestmesh; chestmesh.LoadObject("../models/chest/chest.obj"); chestmesh.SetMeshTextures(chest_textures);

	ModelMesh doormesh; doormesh.LoadObject("../models/door/door.obj"); doormesh.SetMeshTextures(door_textures);


	PlaneMesh floor_mesh; floor_mesh.Init(); floor_mesh.SetMeshTextures(floor_textures);



	//Init Objects

	door = new PBRTexturedObject(doormesh.GetMesh());
	door->transform.Scale(0.15f);
	door->transform.y_angle = 270.0f;
	door->transform.position = glm::vec3(-10, 7.2, -29.7);



	window = new PBRObject(window_mesh.GetMesh());
	window->SetPBRProperties(glm::vec3(144.0f/255.0f,89.0f/255.0f, 35.0f/255.0f), 0.2f, 0.3f); 
	window->transform.Scale(2.0f);
	window->transform.scale.y + 0.1f;

	

	/*ModelMesh candle_mesh;
	candle_mesh.SetMeshTextures(candle_textures);
	candle_mesh.LoadObject("../models/candle-holder/candle.obj");*/

	//CreateTable();

	

	GLfloat end = glfwGetTime();
	std::cout << "Load objects in: " << (end - start) << " seconds" << std::endl;

	//CREATE OBJECTS
	
	
	chandelier = new PBRTexturedObject(chandeliermesh.GetMesh());
	chandelier->transform.Scale(0.4f);
	
	chest = new PBRTexturedObject(chestmesh.GetMesh());
	chest->transform.Scale(4.5f);
	
	

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
	floorplane->transform.Scale(glm::vec3(10.0f,1.0f,10.0f));

	floor_mesh.SetMeshTextures(wall_textures);
	wallplane = new PBRTexturedObject(floor_mesh.GetMesh());
	wallplane->transform.Scale(glm::vec3(4, 1, 4));
	wallplane->transform.Rotate(90, Transform::Z);


	//Init lists
	/*texture_reflect_objects.push_back(candle_holder1);
	texture_reflect_objects.push_back(candle_holder2);*/

	//configure shaders
	//pbr_texture_reflection_shader->UseShader();
	//pbr_texture_reflection_shader->SetInt("reflection_cube", 7);


	pbr_texture_shader->UseShader();
	pbr_texture_shader->SetInt("shadow_map", 10);
	pbr_texture_shader->SetFloat("far_plane", 300.0f);

	pbr_shader->UseShader();
	pbr_shader->SetInt("shadow_map", 10);
	pbr_shader->SetFloat("far_plane", 300.0f);


	//init lighting
	CubeMesh spheremesh;
	spheremesh.Init();
	moveable_light = new LightSource(spheremesh.GetMesh(),LightSource::Point);
	moveable_light->InitShadowMap(SHADOW_RESOLUTION);
	moveable_light->Hide(false);
	moveable_light->SetColour(glm::vec3(0.89f));


	scene_lighting.AddPointLightSource(moveable_light);
	

	//skybox

	const std::vector<std::string> skybox_faces = { "../skybox/right.png","../skybox/left.png" ,"../skybox/up.png","../skybox/down.png","../skybox/back.png","../skybox/front.png"};
	skybox = new Skybox(skybox_faces, skybox_shader);
	skybox->Init();
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

void DrawRoom(Shader* shader)
{
	glm::vec3 start;

	//chandileir
	start = glm::vec3(0, 17, 0);
	for (int i = -1; i < 2; i++)
	{
		chandelier->transform.position = start + glm::vec3(0, 0, i * 13);
		chandelier->Draw(shader);
	}


	//floor
	floorplane->transform.x_angle = 0;
	for (int i = 0; i < 2; i++)
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
		floorplane->transform.x_angle = 180;
	}

	//walls

	//two sides
	start = glm::vec3(0, 4, 26);
	wallplane->transform.y_angle = 0;
	for (int x = -1; x < 2; x += 2) {
		wallplane->transform.y_angle += 180;
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
			wallplane->transform.position = start + glm::vec3(-8 * i, j * 8, 0);
			wallplane->Draw(shader);
		}

	}


	wallplane->transform.y_angle = 270;
	start = glm::vec3(16, 4, 30);
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			wallplane->transform.position = start + glm::vec3(-8 * i, j * 19.4, 0);
			wallplane->Draw(shader);
		}
	}

	glm::vec3 tmp_scale = wallplane->transform.scale;


	start = glm::vec3(0, 9.9, 30);
	wallplane->transform.scale = glm::vec3(1.9, 1, 3);


	for (int i = 0; i < 3; i++)
	{
		for (int j = -1; j < 2; j += 2) {
			wallplane->transform.position = start + glm::vec3(j * -18, 3.8 * i, 0);
			wallplane->Draw(shader);
		}

		for (int j = -1; j < 2; j += 2) {
			wallplane->transform.position = start + glm::vec3(j * -6, 3.8 * i, 0);
			wallplane->Draw(shader);
		}
	}
	wallplane->transform.scale = tmp_scale;
}

void RenderPBRTexture(glm::mat4 projection, glm::mat4 view, glm::vec3 camera_pos, Shader * shader)
{
	shader->UseShader();
	shader->SetMat4("projection", projection);
	shader->SetMat4("view", view);
	shader->SetVec3("camera_position", camera_pos);

	

	//tabletop->Draw(shader);
	//tablebot->Draw(shader);

	chest->transform.y_angle = 90;
	glm::vec3 start = glm::vec3(0, 2.2, 0);

	for (int i = -1; i < 2; i+=2)
	{
		for (int j = -1; j < 2; j++)
		{
			chest->transform.position = start + glm::vec3(i * -17, 0, j * 12);
			chest->Draw(shader);
		}
		
		chest->transform.y_angle = 270;
	}
	
	door->Draw(shader);

	DrawRoom(shader);

	
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

}

void RenderLights(glm::mat4 projection, glm::mat4 view, glm::vec3 camera_pos, Shader* shader)
{
	shader->UseShader();
	shader->SetMat4("projection", projection);
	shader->SetMat4("view", view);
	shader->SetVec3("camera_position", camera_pos);

	for (const auto& light : scene_lighting.GetPointLightSources())
	{
		shader->SetVec3("albedo", light->GetColour());
		light->Draw(shader);
	}

	for (const auto& light : scene_lighting.GetDirectionalLightSources())
	{
		shader->SetVec3("albedo", light->GetColour());
		light->Draw(shader);
	}
}

void RenderSkybox(glm::mat4 projection, glm::mat4 view)
{

	skybox_shader->UseShader();
	skybox_shader->SetMat4("view", glm::mat4(glm::mat3(view)));
	skybox_shader->SetMat4("projection", projection);
	skybox->Draw();
}

void RenderScene(glm::mat4 projection, glm::mat4 view, glm::vec3 camera_pos)
{
	RenderPBRTextureReflect(projection, view, camera_pos, pbr_texture_reflection_shader);
	RenderPBRTexture(projection, view, camera_pos, pbr_texture_shader);
	RenderPBR(projection, view, camera_pos, pbr_shader);
	RenderLights(projection, view, camera_pos, simple_shader);
	RenderSkybox(projection, view);
}




void RenderSceneDepth(glm::mat4 projection, glm::mat4 view, glm::vec3 camera_pos, Shader * shader)
{
	RenderPBRTextureReflect(projection, view, camera_pos, shader);
	RenderPBRTexture(projection, view, camera_pos, shader);
	RenderPBR(projection, view, camera_pos, shader);
	RenderLights(projection, view, camera_pos, shader);
}

void PerformLighting()
{
	scene_lighting.UpdateShaderWithLightInfo(pbr_texture_shader);
	scene_lighting.UpdateShaderWithLightInfo(pbr_shader);

	scene_lighting.RenderPointLightShadows([](glm::mat4 projection, glm::mat4 view, glm::vec3 camera_pos) {RenderSceneDepth(projection, view, camera_pos, point_shadow_shader); }, point_shadow_shader);
	scene_lighting.AttachShadowMaps();	
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

	
	//LIGHTING
	PerformLighting();

	//RENDER REFLECTIONS;
	RenderReflections();

	glViewport(0, 0, screen_width, screen_height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//PROPER DRAW
	glm::mat4 projection = glm::perspective(glm::radians(80.f), aspect_ratio, 0.1f, 300.0f);

	RenderScene(projection, camera.GetView(), camera.GetPosition());
	
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
