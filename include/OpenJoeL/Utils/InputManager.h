//Joe Riemersma

#pragma once
#include <glload/gl_4_0.h>
#include <glload/gl_load.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <utility>
#include <functional>

class InputManager
{
public:

	InputManager(GLFWwindow * window);
	~InputManager();

	void ProcessInput();
	void AddKey(int key, std::function<void()> callback);
	void SetMouseCallback(void(*func)(GLFWwindow* window, double xpos, double ypos));

private:
	GLFWwindow * m_window;
	std::vector <std::pair<int, std::function<void()>>> m_keys;
	std::function<void(double xpos, double ypos)> m_mouse_callback;

	

	

};