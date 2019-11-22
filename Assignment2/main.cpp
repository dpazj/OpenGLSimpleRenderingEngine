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

CubeMesh* blue;
SphereMesh* red;


Camera camera(glm::vec3(0, 0, 5));

InputManager* input_manager;

//DynamicCubemap* dynamic;

GLfloat delta_time = 0;
GLfloat last_frame = 0;


glm::vec3 lightPositions[] = {
	glm::vec3(-10.0f,  10.0f, 10.0f)
};
glm::vec3 lightColors[] = {
	glm::vec3(300.0f, 300.0f, 300.0f)
};

unsigned int loadTexture(char const* path);


GLuint cube, fbo;



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


void create_dynamic_framebuffers()
{

	glActiveTexture(GL_TEXTURE7);
	glGenTextures(1, &cube);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cube);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// set textures
	for (int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	GLuint rbo;
	glGenFramebuffers(1, &fbo);
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1024, 1024);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fbo);


	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, cube, 0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	switch (status) {
		case GL_FRAMEBUFFER_UNDEFINED: {
			fprintf(stderr, "Undefined.\n");
			break;
		}
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: {
			fprintf(stderr, "FBO: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT.\n");
			break;
		}
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: {
			fprintf(stderr, "FBO: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT.\n");
			break;
		}
		case GL_FRAMEBUFFER_UNSUPPORTED: {
			fprintf(stderr, "FBO: GL_FRAMEBUFFER_UNSUPPORTED.\n");
			break;
		}
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: {
			fprintf(stderr, "FBO: GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE.\n");
			break;
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glActiveTexture(GL_TEXTURE0);
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

	
	red = new SphereMesh();
	blue= new CubeMesh();

	red ->Init();
	blue ->Init();


	//dynamic = new DynamicCubemap(1024,1024);

	skybox_shader->UseShader();
	skybox_shader->SetInt("skybox", 1);

	reflection_shader->UseShader();
	reflection_shader->SetInt("reflection_cube", 0);

	const std::vector<std::string> skybox_paths = { "../skybox/right.png","../skybox/left.png" ,"../skybox/up.png" ,"../skybox/down.png","../skybox/back.png","../skybox/front.png" };
	skybox = new Skybox(skybox_paths, skybox_shader);
	skybox->Init();


	create_dynamic_framebuffers();
}



void RenderScene(glm::mat4 projection, glm::mat4 view)
{
	shader->UseShader();
	shader->SetMat4("view", view);
	shader->SetMat4("projection", projection);
	shader->SetVec3("camera_position", camera.GetPosition());
	shader->SetFloat("metallic", 1.0f);
	shader->SetFloat("roughness", 0.2f);
	shader->SetFloat("ambient_occlusion", 1.0f);

	reflection_shader->UseShader();
	reflection_shader->SetMat4("view", view);
	reflection_shader->SetMat4("projection", projection);
	reflection_shader->SetVec3("camera_position", camera.GetPosition());
	reflection_shader->SetFloat("metallic", 1.0f);
	reflection_shader->SetFloat("roughness", 0.2f);
	reflection_shader->SetFloat("ambient_occlusion", 1.0f);


	auto model = glm::mat4(1.0f);



	glBindTexture(GL_TEXTURE_CUBE_MAP, cube);
	model = glm::scale(model, glm::vec3(3, 3, 3));
	model = glm::translate(model, glm::vec3(-2, 0, 0));
	reflection_shader->SetMat4("model", model);
	reflection_shader->SetVec3("albedo", glm::vec3(0, 0, 1));
	blue->Draw(reflection_shader);


	model = glm::mat4(1.0f);
	shader->UseShader();
	model = glm::translate(model, glm::vec3(2, 0, 0));
	shader->SetMat4("model", model);
	shader->SetVec3("albedo", glm::vec3(1, 0, 0));
	red->Draw(shader);

	for (unsigned int i = 0; i < 4; ++i)
	{
		glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
		newPos = lightPositions[i];
		shader->SetVec3(("light_positions[" + std::to_string(i) + "]").c_str(), newPos);
		shader->SetVec3(("light_colors[" + std::to_string(i) + "]").c_str(), lightColors[i]);
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


	/*--------------------*/
	glActiveTexture(GL_TEXTURE7);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cube);

	const auto get_lookat = [](GLfloat pitch, GLfloat yaw, glm::vec3 position)
	{
		const glm::vec3 world_up(0.0f, 1.0f, 0.0f);
		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(front);

		const auto right = glm::normalize(glm::cross(front, world_up));
		const auto up = glm::normalize(glm::cross(right, front));

		return glm::lookAt(position, position + front, up);
	};

	for (int face = 0; face < 6; face++)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, cube, 0);
		GLfloat pitch = 0;
		GLfloat yaw = 0;
		const glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);

		switch (face)
		{
			case 0:
				pitch = 0;
				yaw = 90;
				break;
			case 1:
				pitch = 0;
				yaw = -90;
				break;
			case 2:
				pitch = -90;
				yaw = 180;
				break;
			case 3:
				pitch = 90;
				yaw = 180;
				break;
			case 4:
				pitch = 0;
				yaw = 180;
				break;
			case 5:
				pitch = 0;
				yaw = 0;
				break;
		};
		RenderScene(projection, get_lookat(pitch, yaw, glm::vec3(-3.5,0,0) ));
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(GL_TEXTURE0);

	/*--------------------*/

	const glm::mat4 projection = glm::perspective(glm::radians(30.0f), aspect_ratio, 0.1f, 100.0f);
	const glm::mat4 view = camera.GetView();
	RenderScene(projection, view);

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
