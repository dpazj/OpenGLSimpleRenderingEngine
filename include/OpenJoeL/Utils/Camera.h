//Joe Riemersma

#pragma once
#include <glload/gl_4_0.h>
#include <glload/gl_load.h>

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>


enum Direction
{
	FORWARD,
	BACK,
	LEFT,
	RIGHT,
};


class Camera
{
public:
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = -90.0f, GLfloat pitch = 0.0f, GLfloat speed = 10.0f, GLfloat sensitivity = 0.1f);
	~Camera();
	void ProcessKeyboard(Direction direction, GLfloat delta);
	void ProcessMouse(GLfloat xoffset, GLfloat yoffset, GLboolean constrain_pitch = true);

	glm::mat4 GetView();
	glm::vec3 GetPosition();

private:
	glm::vec3 m_position;
	glm::vec3 m_front;
	glm::vec3 m_world_up;
	glm::vec3 m_up;
	glm::vec3 m_right;
	glm::vec3 m_target;

	GLfloat m_speed;
	GLfloat m_yaw;
	GLfloat m_pitch;
	GLfloat m_sensitivity;

	//Mouse variables
	bool m_first_mouse = true;
	bool m_double_speed;
	GLfloat last_x = 0;
	GLfloat last_y = 0;

	void UpdateVectors();

};