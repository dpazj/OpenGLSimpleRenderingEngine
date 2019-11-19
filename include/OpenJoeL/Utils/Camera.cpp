//Joe Riemersma 
#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch, GLfloat speed, GLfloat sensitivity)
{
	m_world_up = up;
	m_position = position;

	m_yaw = yaw;
	m_pitch = pitch;
	m_speed = speed;
	m_sensitivity = sensitivity;

	this->UpdateVectors();
}
Camera::~Camera()
{

}

void Camera::ProcessKeyboard(Direction direction, GLfloat delta) 
{
	GLfloat speed = delta * m_speed;
	if (direction == FORWARD) m_position +=  m_front * speed;
	if (direction == BACK) m_position -=  m_front * speed;
	if (direction == LEFT) m_position -= m_right * speed;
	if (direction == RIGHT) m_position += m_right * speed;
	this->UpdateVectors();
}

void Camera::ProcessMouse(GLfloat xpos, GLfloat ypos, GLboolean constrain_pitch)
{
	if (m_first_mouse)
	{
		last_x = xpos;
		last_y = ypos;
		m_first_mouse = false;
	}

	GLfloat xoffset = xpos - last_x;
	GLfloat yoffset = last_y - ypos;
	last_x = xpos;
	last_y = ypos;

	xoffset *= m_sensitivity;
	yoffset *= m_sensitivity;

	m_yaw = glm::mod(m_yaw + xoffset,360.0f);
	m_pitch += yoffset;

	//keep screen in bounds
	if (constrain_pitch)
	{
		if (m_pitch > 89.0f)
			m_pitch = 89.0f;
		if (m_pitch < -89.0f)
			m_pitch = -89.0f;
	}

	UpdateVectors();
}

glm::mat4 Camera::GetView()
{
	return glm::lookAt(m_position, m_position + m_front, m_up);
}

glm::vec3 Camera::GetPosition() { return m_position; }

//adapted from https://learnopengl.com/Getting-started/Camera
void Camera::UpdateVectors() 
{
	glm::vec3 front;
	front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	front.y = sin(glm::radians(m_pitch));
	front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_front = glm::normalize(front);

	m_right = glm::normalize(glm::cross(m_front, m_world_up));
	m_up = glm::normalize(glm::cross(m_right, m_front));
}