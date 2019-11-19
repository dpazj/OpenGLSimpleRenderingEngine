//Joe Riemersma

#include "InputManager.h"

#include <iostream>

InputManager::InputManager(GLFWwindow* window)
{
	m_window = window;
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

InputManager::~InputManager()
{

}

void InputManager::AddKey(int key, std::function<void()> function)
{
	m_keys.push_back(std::pair<int,std::function<void()>>(key, function));
}

void InputManager::ProcessInput()
{
	for (auto const& key : m_keys)
	{
		if (glfwGetKey(m_window, key.first) == GLFW_PRESS)
		{
			key.second();
		}
	}
}


void InputManager::SetMouseCallback(void(*callback)(GLFWwindow* window, double xpos, double ypos))
{
	glfwSetCursorPosCallback(m_window, callback);
}