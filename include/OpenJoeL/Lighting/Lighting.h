//Joe Riemersma

#pragma once
#include "LightSource.h"
#include <vector>

#define MAX_LIGHTS 10

class Lighting
{
public:

	Lighting();

	void AddLightSource(LightSource* light_source);
	std::vector<LightSource*> GetLightSources();
	void UpdateShader(Shader * shader);

	void RenderPointLights(Shader * shader);
	void RenderDirectionalLights(Shader* shader);
	

private:

	std::vector <LightSource*> m_lights;
};