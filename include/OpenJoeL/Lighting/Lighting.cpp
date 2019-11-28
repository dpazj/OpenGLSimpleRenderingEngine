//Joe Riemersma

#include "Lighting.h"
#include <string>
#include <iostream>
Lighting::Lighting()
{

}

void Lighting::AddLightSource(LightSource* light_source)
{
	if (m_lights.size() > MAX_LIGHTS) { return; }
	m_lights.push_back(light_source);
}

std::vector <LightSource*> Lighting::GetLightSources()
{
	return m_lights;
}

void Lighting::UpdateShader(Shader* shader)
{
	int counter = 0;
	shader->UseShader();
	for (const auto& light : m_lights)
	{
		const std::string name_index("lights[" + std::to_string(counter) + "].");

		shader->SetVec3((name_index + "position").c_str(), light->transform.position);
		shader->SetVec3((name_index + "colour").c_str(), light->GetColour());

		counter++;
	}
	shader->SetUInt("number_of_lights", (GLuint) m_lights.size());
}



void Lighting::RenderPointLights(Shader* shader){}
void Lighting::RenderDirectionalLights(Shader* shader){}