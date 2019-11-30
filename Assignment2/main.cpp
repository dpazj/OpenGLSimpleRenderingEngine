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
Shader* pbr_reflect_shader;
Shader* pbr_texture_shader;
Shader* pbr_texture_reflection_shader;
Shader* simple_shader;
Shader* skybox_shader;

//Objects
//room
PBRTexturedObject* floorplane;
PBRTexturedObject* wallplane;
PBRTexturedObject* tabletop;
PBRTexturedObject* tablebot;
PBRTexturedObject* chandelier;
PBRTexturedObject* door;
PBRTexturedObject* torch;

//Objects
PBRTexturedReflectObject* gold_clock;

PBRTexturedObject* chest;

PBRObject* window;


PBRReflectObject* reflection_circle;


Skybox* skybox;


std::vector<PBRTexturedReflectObject*> texture_reflect_objects;
std::vector<PBRReflectObject*> reflect_objects;


//Lighting
Lighting scene_lighting;
LightSource* moveable_light;
LightSource* sun_light;


//OTHER
Camera camera(glm::vec3(0, 2, 5));
InputManager* input_manager;

GLfloat delta_time = 0;
GLfloat last_frame = 0;

bool hide_sun = false;
bool hide_light, hide_reflection_sphere = true;
GLfloat tmp_power = 0; GLfloat sun_tmp_power = 0;

unsigned int loadTexture(char const* path);
void RenderScene(glm::mat4 projection, glm::mat4 view, glm::vec3 camera_pos);
void RenderSceneDepth(glm::mat4 projection, glm::mat4 view, glm::vec3 camera_pos, Shader* shader);

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
		moveable_light->transform.position.x += 10 * delta_time;
	});
	input_manager->AddKey(GLFW_KEY_L, [glw]() {
		moveable_light->transform.position.x -= 10 * delta_time;
	});
	input_manager->AddKey(GLFW_KEY_I, [glw]() {
		moveable_light->transform.position.z += 10 * delta_time;
	});
	input_manager->AddKey(GLFW_KEY_K, [glw]() {
		moveable_light->transform.position.z -= 10 * delta_time;
	});
	input_manager->AddKey(GLFW_KEY_U, [glw]() {
		moveable_light->transform.position.y += 10 * delta_time;
	});
	input_manager->AddKey(GLFW_KEY_O, [glw]() {
		moveable_light->transform.position.y -= 10 * delta_time;
	});

	

	input_manager->AddKey(GLFW_KEY_0, [glw]() {

		moveable_light->SetPower(moveable_light->GetPower() + 5.0f);
	});

	input_manager->AddKey(GLFW_KEY_9, [glw]() {
		if (moveable_light->GetPower() > 0.0f)
		{
			moveable_light->SetPower(moveable_light->GetPower() - 5.0f);
		}
	});


}




void SetupLighting()
{
	SphereMesh spheremesh;
	spheremesh.Init();


	moveable_light = new LightSource(spheremesh.GetMesh(), LightSource::Point);
	moveable_light->InitShadowMap(SHADOW_RESOLUTION);
	moveable_light->Hide(true);
	moveable_light->SetColour(glm::vec3(0.89f));
	moveable_light->SetShadowBias(0.45f); 
	moveable_light->SetPower(0.0f);
	moveable_light->transform.position = glm::vec3(0,12,0);


	sun_light = new LightSource(spheremesh.GetMesh(), LightSource::Point);
	sun_light->InitShadowMap(SHADOW_RESOLUTION * 2);
	sun_light->Hide(false);
	sun_light->SetColour(glm::vec3(0.89f));
	sun_light->SetShadowBias(0.45f);
	sun_light->SetPower(130.0f);
	sun_light->SetShadowStrength(0.7f);

	glm::vec3 start(0, 16.6, 0);
	//setup torch lights
	for (int i = -1; i < 2; i+=2)
	{
		for (int j = -1; j < 2; j++)
		{
			LightSource* torchlight = new LightSource(spheremesh.GetMesh(), LightSource::Point);
			torchlight->InitShadowMap(SHADOW_RESOLUTION);
			torchlight->Hide(false);
			torchlight->transform.Scale(0.2f);
			torchlight->SetColour(glm::vec3(0.76,0.278,0));
			torchlight->SetPower(30.0f);
			torchlight->SetShadowStrength(1.0f);
			torchlight->SetShadowBias(0.60f);
			torchlight->transform.position = start + glm::vec3(i * -18.9, 0, j * 15);
			scene_lighting.AddPointLightSource(torchlight);
		}
	}

	scene_lighting.AddPointLightSource(moveable_light);
	scene_lighting.AddPointLightSource(sun_light);



	scene_lighting.RenderStaticPointLightShadows([](glm::mat4 projection, glm::mat4 view, glm::vec3 camera_pos) {RenderSceneDepth(projection, view, camera_pos, point_shadow_shader); }, point_shadow_shader);
}

void init(GLWrapper* glw)
{

	//load shaders
	try
	{

		pbr_shader = new Shader("../shaders/pbr.vert", "../shaders/pbr.frag");
		point_shadow_shader = new Shader("../shaders/pointshadow.vert", "../shaders/pointshadow.frag", "../shaders/pointshadow.gs");
		pbr_reflect_shader = new Shader("../shaders/pbrreflection.vert", "../shaders/pbrreflection.frag");
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
	std::cout << "Loading objects and textures... May take upwards of 15 seconds :( " << std::endl;

	//LOAD TEXTURES
	
	//PBRTextures candle_textures("../models/candle-holder/textures/albedo.png", "../models/candle-holder/textures/normal.png", "../models/candle-holder/textures/metallic.png", "../models/candle-holder/textures/roughness.png", "../models/candle-holder/textures/ao.png");
	PBRTextures floor_textures("../models/floor/albedo.png", "../models/floor/normal.png", "../models/floor/metallic.psd", "../models/floor/roughness.png", "../models/floor/ao.png");
	PBRTextures wall_textures("../models/wall/albedo.png", "../models/wall/normal.png", "../models/wall/metallic.png", "../models/wall/roughness.png", "../models/wall/ao.png");
	PBRTextures chandelier_textures("../models/chandelier/textures/albedo.png", "../models/chandelier/textures/normal.png", "../models/chandelier/textures/metallic.png", "../models/chandelier/textures/roughness.png", "../models/chandelier/textures/ao.png");
	PBRTextures chest_textures("../models/chest/textures/albedo.jpeg", "../models/chest/textures/normal.jpeg", "../models/chest/textures/metallic.jpeg", "../models/chest/textures/roughness.jpeg", "../models/chest/textures/ao.jpeg");
	PBRTextures door_textures("../models/door/textures/albedo.png", "../models/door/textures/normal.png", "../models/door/textures/roughness.png", "../models/door/textures/roughness.png", "../models/door/textures/ao.png");
	PBRTextures torch_textures("../models/torch/textures/albedo.png", "../models/torch/textures/normal.jpg", "../models/torch/textures/metallic.jpg", "../models/torch/textures/roughness.png", "../models/torch/textures/ao.jpg");
	PBRTextures clock_textures("../models/clock/textures/albedo.jpg", "../models/clock/textures/normal.jpg", "../models/clock/textures/metallic.jpg", "../models/clock/textures/roughness.jpg", "../models/clock/textures/ao.jpg");

	//LOAD MESHES
	ModelMesh chandeliermesh; chandeliermesh.LoadObject("../models/chandelier/Chandelier.fbx"); chandeliermesh.SetMeshTextures(chandelier_textures);
	ModelMesh window_mesh; window_mesh.LoadObject("../models/window/window.obj");
	ModelMesh chestmesh; chestmesh.LoadObject("../models/chest/chest.obj"); chestmesh.SetMeshTextures(chest_textures);
	ModelMesh torchmesh; torchmesh.LoadObject("../models/torch/torch.obj"); torchmesh.SetMeshTextures(torch_textures);
	ModelMesh doormesh; doormesh.LoadObject("../models/door/door.obj"); doormesh.SetMeshTextures(door_textures);
	ModelMesh clockmesh; clockmesh.LoadObject("../models/clock/clock.obj"); clockmesh.SetMeshTextures(clock_textures);

	SphereMesh spheremesh; spheremesh.Init();
	PlaneMesh floor_mesh; floor_mesh.Init(); floor_mesh.SetMeshTextures(floor_textures);

	//CREATE Objects


	gold_clock = new PBRTexturedReflectObject(clockmesh.GetMesh());
	gold_clock->CreateDynamicCubeMap(REFLECTION_RESOLUTION);
	gold_clock->transform.y_angle = 180;
	gold_clock->transform.position = glm::vec3(0,0,25);
	gold_clock->AddOffset(glm::vec3(0, 3, 0));

	reflection_circle = new PBRReflectObject(spheremesh.GetMesh());
	reflection_circle->CreateDynamicCubeMap(REFLECTION_RESOLUTION * 2);
	reflection_circle->SetPBRProperties(glm::vec3(0, 0, 1), 1.0f, 0.05f);
	reflection_circle->Hide(true);
	reflection_circle->transform.Scale(4.0f);
	reflection_circle->transform.position = glm::vec3(0, 5, 0);

	torch = new PBRTexturedObject(torchmesh.GetMesh());
	torch->transform.Scale(2.5f);
	torch->transform.z_angle = -15.0f;

	door = new PBRTexturedObject(doormesh.GetMesh());
	door->transform.Scale(0.15f);
	door->transform.y_angle = 270.0f;
	door->transform.position = glm::vec3(-10, 7.2, -29.8);

	window = new PBRObject(window_mesh.GetMesh());
	window->SetPBRProperties(glm::vec3(144.0f/255.0f,89.0f/255.0f, 35.0f/255.0f), 0.2f, 0.3f); 
	window->transform.Scale(2.0f);
	window->transform.scale.x += 0.2f;
	window->transform.scale.y += 0.2f;




	

	GLfloat end = glfwGetTime();
	std::cout << "Load objects and textures in: " << (end - start) << " seconds" << std::endl;

	//CREATE OBJECTS
	
	
	chandelier = new PBRTexturedObject(chandeliermesh.GetMesh());
	chandelier->transform.Scale(0.4f);
	
	chest = new PBRTexturedObject(chestmesh.GetMesh());
	chest->transform.Scale(4.5f);
	
	

	floorplane = new PBRTexturedObject(floor_mesh.GetMesh());
	floorplane->transform.Scale(glm::vec3(10.0f,1.0f,10.0f));

	floor_mesh.SetMeshTextures(wall_textures);
	wallplane = new PBRTexturedObject(floor_mesh.GetMesh());
	wallplane->transform.Scale(glm::vec3(4, 1, 4));
	wallplane->transform.Rotate(90, Transform::Z);


	//Init lists

	
	texture_reflect_objects.push_back(gold_clock);
	reflect_objects.push_back(reflection_circle);


	//configure shaders


	pbr_shader->UseShader();
	pbr_shader->SetInt("shadow_map", 10);
	pbr_shader->SetFloat("far_plane", 300.0f);

	pbr_texture_reflection_shader->UseShader();
	pbr_texture_reflection_shader->SetInt("reflection_cube", 7);

	pbr_reflect_shader->UseShader();
	pbr_reflect_shader->SetInt("reflection_cube", 7);
	//init lighting

	SetupLighting();

	//skybox

	const std::vector<std::string> skybox_faces = { "../skybox/right.png","../skybox/left.png" ,"../skybox/up.png","../skybox/down.png","../skybox/back.png","../skybox/front.png"};
	skybox = new Skybox(skybox_faces, skybox_shader);
	skybox->Init();
}

void RenderReflections() 
{
	for (auto obj : texture_reflect_objects)
	{
		bool x = obj->IsHidden();
		obj->Hide(true);
		glm::vec3 pos = obj->transform.position + obj->GetOffset();
		obj->RenderCubemap([pos](glm::mat4 projection, glm::mat4 view) {RenderScene(projection, view, pos);});
		obj->Hide(x);
	}

	for (auto obj : reflect_objects)
	{
		bool x = obj->IsHidden();
		obj->Hide(true);
		glm::vec3 pos = obj->transform.position;
		obj->RenderCubemap([pos](glm::mat4 projection, glm::mat4 view) {RenderScene(projection, view, pos); });
		obj->Hide(x);
	}
}


void RenderPBRTextureReflect(glm::mat4 projection, glm::mat4 view, glm::vec3 camera_pos, Shader* shader)
{
	shader->UseShader();
	shader->SetMat4("projection", projection);
	shader->SetMat4("view", view);
	shader->SetVec3("camera_position", camera_pos);

	//candle_holder1->Draw(shader);
	//candle_holder2->Draw(shader);
	gold_clock->Draw(shader);
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
	wallplane->transform.scale = glm::vec3(2, 1, 2.9);


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
	glm::vec3 start;
	
	
	start = glm::vec3(0, 15, 0);
	torch->transform.y_angle = 180;
	for (int i = -1; i < 2; i += 2)
	{
		for (int j = -1; j < 2; j++)
		{
			torch->transform.position = start + glm::vec3(i * -19.5,0, j * 15);
			torch->Draw(shader);
		}
		torch->transform.y_angle = 0;
	}


	//tabletop->Draw(shader);
	//tablebot->Draw(shader);

	chest->transform.y_angle = 90;
	start = glm::vec3(0, 2.2, 0);

	for (int i = -1; i < 2; i+=2)
	{
		for (int j = -1; j < 2; j++)
		{
			chest->transform.position = start + glm::vec3(i * -17.4, 0, j * 12);
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

void RenderPBRReflect(glm::mat4 projection, glm::mat4 view, glm::vec3 camera_pos, Shader* shader)
{
	shader->UseShader();
	shader->SetMat4("projection", projection);
	shader->SetMat4("view", view);
	shader->SetVec3("camera_position", camera_pos);

	reflection_circle->Draw(shader);
}

void RenderScene(glm::mat4 projection, glm::mat4 view, glm::vec3 camera_pos)
{
	RenderPBRTextureReflect(projection, view, camera_pos, pbr_texture_reflection_shader);
	RenderPBRTexture(projection, view, camera_pos, pbr_texture_shader);
	RenderPBRReflect(projection, view, camera_pos, pbr_reflect_shader);
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
	scene_lighting.UpdateShaderWithLightInfo(pbr_texture_reflection_shader);

	scene_lighting.RenderPointLightShadows([](glm::mat4 projection, glm::mat4 view, glm::vec3 camera_pos) {RenderSceneDepth(projection, view, camera_pos, point_shadow_shader); }, point_shadow_shader);
	scene_lighting.AttachShadowMaps();	
}


void display()
{
	const GLfloat currentFrame = (GLfloat)glfwGetTime();
	delta_time = currentFrame - last_frame;
	last_frame = currentFrame;


	sun_light->transform.position = glm::vec3(0, 15, 60) + glm::vec3((GLfloat) sin(glfwGetTime() * 0.05) * 15, 0, (GLfloat) cos(glfwGetTime() * 0.05) * 15);
	


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

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	
	if (key == GLFW_KEY_T && action == GLFW_PRESS)
	{
		hide_light = !hide_light;
		if (hide_light) { tmp_power = moveable_light->GetPower(); moveable_light->SetPower(0); }
		else{ moveable_light->SetPower(50.0f); }
		moveable_light->Hide(hide_light);
	}
		
	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		hide_reflection_sphere = !hide_reflection_sphere;
		reflection_circle->Hide(hide_reflection_sphere);
	}
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		hide_sun = !hide_sun;
		if (hide_sun) { sun_tmp_power = sun_light->GetPower(); sun_light->SetPower(0); }
		else { sun_light->SetPower(sun_tmp_power); }
		sun_light->Hide(hide_sun);
	}

}

void print_instructions()
{
	std::cout << std::endl << std::endl << "WASD: Move camera positon" << std::endl;

	std::cout << "Mouse: Camera look around" << std::endl;
	std::cout << "IJ KL UO: Move Light around" << std::endl;
	std::cout << "T: Toggle light on/off" << std::endl;
	std::cout << "9 0: Increase decrease light strength" << std::endl;

	std::cout << "Y: Toggle reflection sphere on/off" << std::endl;
	std::cout << "R: Toggle Sun" << std::endl;

	



}

/* Entry point of program */
int main(int argc, char* argv[])
{
	GLWrapper* glw = new GLWrapper(screen_width, screen_height, "Assignment 2 - Joe Riemersma");
	input_manager = new InputManager(glw->getWindow());

	if (!ogl_LoadFunctions())
	{
		fprintf(stderr, "ogl_LoadFunctions() failed. Exiting\n");
		return 0;
	}

	// Register the callback functions
	glw->setKeyCallback(key_callback);
	glw->setRenderer(display);
	glw->setReshapeCallback(reshape);

	/* Output the OpenGL vendor and version */
	glw->DisplayVersion();
	print_instructions();

	init(glw);

	glw->eventLoop();

	delete(glw);
	return 0;
}
