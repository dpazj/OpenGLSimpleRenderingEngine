//Joe Riemersma

#pragma once
#include "LightSource.h"
#include <vector>

#define MAX_LIGHTS 10

class Lighting
{
public:

	Lighting();

	void AddPointLightSource(LightSource* light_source);
	void AddDirectionalLightSource(LightSource* light_source);

	std::vector<LightSource*> GetPointLightSources();
	std::vector<LightSource*> GetDirectionalLightSources();

	void UpdateShaderWithLightInfo(Shader * shader);


	void RenderPointLightShadows(std::function<void(glm::mat4, glm::mat4, glm::vec3)> render_function, Shader* shader);
	//void RenderDirectionalLightShadows(std::function<void(glm::mat4, glm::mat4)> render_function, Shader* shader);

	void AttachShadowMaps();

private:

	std::vector <LightSource*> m_point_lights;
	std::vector <LightSource*> m_directional_lights;

	const GLint SHADOW_MAP_START_INDEX = 10;

};