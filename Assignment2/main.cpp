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

#include "OpenJoeL/Meshes/Object.h"

#include "OpenJoeL/Texture/Texture.h"

#include "OpenJoeL/Utils/Camera.h"
#include "OpenJoeL/Utils/InputManager.h"
#include "OpenJoeL/Environment/Skybox.h"

#include "OpenJoeL/Render/DynamicCubemap.h"



// Include headers for our objects

GLint screen_width = 1680;
GLint screen_height = 1050;
GLfloat aspect_ratio = (GLfloat)screen_width / (GLfloat)screen_height;


//SHADERS
//std::shared_ptr<Shader> shader;
Shader* shader;
Shader* reflection_shader;
Shader* skybox_shader;


Skybox* skybox;

PBRReflectObject* red_sphere;
PBRReflectObject* blue_sphere;

Camera camera(glm::vec3(0, 0, 5));

InputManager* input_manager;



bool blue_render = true;

GLfloat delta_time = 0;
GLfloat last_frame = 0;


glm::vec3 lightPositions[] = {
	glm::vec3(-10.0f,  10.0f, 10.0f)
};
glm::vec3 lightColors[] = {
	glm::vec3(300.0f, 300.0f, 300.0f)
};

unsigned int loadTexture(char const* path);


glm::vec3 blue_pos(-3.5,0,0);

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

	input_manager->AddKey(GLFW_KEY_J, []() {
		blue_sphere->transform.position.x -= 0.05f;
	});

	input_manager->AddKey(GLFW_KEY_L, []() {
		blue_sphere->transform.position.x += 0.05f;
	});

	input_manager->AddKey(GLFW_KEY_K, []() {
		blue_sphere->transform.position.y -= 0.05f;
	});

	input_manager->AddKey(GLFW_KEY_I, []() {
		blue_sphere->transform.position.y += 0.05f;
	});

	input_manager->AddKey(GLFW_KEY_U, []() {
		blue_sphere->transform.position.z -= 0.05f;
	});

	input_manager->AddKey(GLFW_KEY_O, []() {
		blue_sphere->transform.position.z += 0.05f;
	});

	

}


void init(GLWrapper* glw)
{

	input_manager->ProcessInput();

	//load shaders
	try
	{
		//shader = new Shader("../shaders/pbrtexture.vert", "../shaders/pbrtexture.frag");
		skybox_shader = new Shader("../shaders/skybox.vert", "../shaders/skybox.frag");
		shader = new Shader("../shaders/pbr.vert", "../shaders/pbr.frag");
		reflection_shader = new Shader("../shaders/pbrreflection.vert", "../shaders/pbrreflection.frag");
	}
	catch (std::exception & e)
	{
		std::cout << "Caught exception: " << e.what() << std::endl;
		std::cin.ignore();
		exit(0);
	}

	setup_inputs(glw);
	shader->UseShader();

	
	SphereMesh sphere;
	sphere.Init();

	
	red_sphere = new PBRReflectObject(sphere.GetMesh());
	red_sphere->CreateDynamicCubeMap(1024);
	red_sphere->SetPBRProperties(glm::vec3(1, 0, 0), 1, 0.2);
	red_sphere->transform.Translate(glm::vec3(3.5, 0, 0));

	blue_sphere = new PBRReflectObject(sphere.GetMesh());
	blue_sphere->CreateDynamicCubeMap(1024);
	blue_sphere->SetPBRProperties(glm::vec3(0, 0, 1), 1, 0.2);
	blue_sphere->transform.Translate(glm::vec3(blue_pos));

	


	skybox_shader->UseShader();
	skybox_shader->SetInt("skybox", 1);

	reflection_shader->UseShader();
	reflection_shader->SetInt("reflection_cube", 10);

	const std::vector<std::string> skybox_paths = { "../skybox/right.png","../skybox/left.png" ,"../skybox/up.png" ,"../skybox/down.png","../skybox/back.png","../skybox/front.png" };
	skybox = new Skybox(skybox_paths, skybox_shader);
	skybox->Init();
}



void RenderScene(glm::vec3 camera_pos, glm::mat4 projection, glm::mat4 view, bool render_both, bool render_blue = true)
{
	shader->UseShader();
	shader->SetMat4("view", view);
	shader->SetMat4("projection", projection);
	shader->SetVec3("camera_position", camera_pos);
	shader->SetFloat("metallic", 1.0f);
	shader->SetFloat("roughness", 0.2f);
	shader->SetFloat("ambient_occlusion", 1.0f);

	reflection_shader->UseShader();
	reflection_shader->SetMat4("view", view);
	reflection_shader->SetMat4("projection", projection);
	reflection_shader->SetVec3("camera_position", camera_pos);

	auto model = glm::mat4(1.0f);
	reflection_shader->UseShader();
	if (render_both || render_blue) {
		
		blue_sphere->Draw(reflection_shader);
	}
	model = glm::mat4(1.0f);
	if (render_both || !render_blue)
	{
		red_sphere->Draw(reflection_shader);
	}

	for (unsigned int i = 0; i < 4; ++i)
	{
		glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
		newPos = lightPositions[i];
		shader->UseShader();
		shader->SetVec3(("light_positions[" + std::to_string(i) + "]").c_str(), newPos);
		shader->SetVec3(("light_colors[" + std::to_string(i) + "]").c_str(), lightColors[i]);

		reflection_shader->UseShader();
		reflection_shader->SetVec3(("light_positions[" + std::to_string(i) + "]").c_str(), newPos);
		reflection_shader->SetVec3(("light_colors[" + std::to_string(i) + "]").c_str(), lightColors[i]);
	}

	{
		skybox_shader->UseShader();
		skybox_shader->SetMat4("view", glm::mat4(glm::mat3(view)));
		skybox_shader->SetMat4("projection", projection);
		skybox->Draw();
	}
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


	if (blue_render)
	{
		blue_sphere->RenderCubemap([](glm::mat4 projection, glm::mat4 view) {
			RenderScene(blue_pos,projection, view, false, false);
		});
	}
	else
	{
		red_sphere->RenderCubemap([](glm::mat4 projection, glm::mat4 view) {
			RenderScene(glm::vec3(3.5,0,0),projection, view, false, true);
		});
	}
	

	glViewport(0, 0, screen_width, screen_height);
	const glm::mat4 view = camera.GetView();
	const glm::mat4 projection = glm::perspective(glm::radians(30.0f), aspect_ratio, 0.1f, 100.0f);
	RenderScene(camera.GetPosition(),projection, view, true);

	blue_render = !blue_render;

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
