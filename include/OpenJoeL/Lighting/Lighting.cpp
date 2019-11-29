//Joe Riemersma

#include "Lighting.h"
#include <string>
#include <iostream>
Lighting::Lighting()
{

}

void Lighting::AddPointLightSource(LightSource* light_source)
{
	if (m_point_lights.size() > MAX_LIGHTS || light_source->GetType() == LightSource::Directional) { return; }
	m_point_lights.push_back(light_source);
}

void Lighting::AddDirectionalLightSource(LightSource* light_source)
{
	if (m_directional_lights.size() > MAX_LIGHTS || light_source->GetType() == LightSource::Point) { return; }
	m_directional_lights.push_back(light_source);
}

std::vector <LightSource*> Lighting::GetPointLightSources()
{
	return m_point_lights;
}

std::vector <LightSource*> Lighting::GetDirectionalLightSources()
{
	return m_directional_lights;
}

void Lighting::UpdateShaderWithLightInfo(Shader* shader)
{
	int counter = 0;
	shader->UseShader();
	for (const auto& light : m_point_lights)
	{
		const std::string name_index("lights[" + std::to_string(counter) + "].");
		
		shader->SetVec3((name_index + "position").c_str(), light->transform.position);
		shader->SetVec3((name_index + "colour").c_str(), light->GetColour());
		shader->SetFloat((name_index + "power").c_str(), light->GetPower());
		shader->SetFloat((name_index + "far_plane").c_str(), light->GetFarPlane()); 
		shader->SetFloat((name_index + "shadow_strength").c_str(), light->GetShadowStrength());
		shader->SetFloat((name_index + "shadow_bias").c_str(), light->GetShadowBias());

		counter++;
	}

	for (const auto& light : m_directional_lights)
	{
		const std::string name_index("lights[" + std::to_string(counter) + "].");

		shader->SetVec3((name_index + "position").c_str(), light->transform.position);
		shader->SetVec3((name_index + "colour").c_str(), light->GetColour());
		shader->SetFloat((name_index + "power").c_str(), light->GetPower());
		shader->SetFloat((name_index + "far_plane").c_str(), light->GetFarPlane());
		shader->SetFloat((name_index + "shadow_strength").c_str(), light->GetShadowStrength());
		shader->SetFloat((name_index + "shadow_bias").c_str(), light->GetShadowBias());

		counter++;
	}


	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		shader->SetInt(("shadow_cube_maps[" + std::to_string(i) + "]").c_str(), SHADOW_MAP_START_INDEX + i); //change this to array
	}
	
	shader->SetUInt("number_of_lights", (GLuint) counter + 1);
}





void Lighting::RenderPointLightShadows(std::function<void(glm::mat4, glm::mat4, glm::vec3)> render_function, Shader * shader)
{
	for (const auto& light : m_point_lights)
	{
		if (light->GetType() == LightSource::Point)
		{
			bool x = light->IsHidden();
			light->Hide(true);
			light->RenderShadowMap(render_function, shader);
			light->Hide(x);
		}
	}
}

//void Lighting::RenderDirectionalLightShadows(std::function<void(glm::mat4, glm::mat4)> render_function, Shader* shader)
//{
//	for (const auto& light : m_lights)
//	{
//		if (light->GetType() == LightSource::Directional)
//		{
//			light->RenderShadowMap(render_function, shader);
//		}
//	}
//}


void Lighting::AttachShadowMaps()
{
	GLuint counter = 0;
	for (const auto& light : m_point_lights)
	{
		if (light->GetType() == LightSource::Point)
		{
			light->BindShadowMap(counter);
			counter++;
		}
	}

	/*for (const auto& light : m_lights)
	{
		if (light->GetType() == LightSource::Directional)
		{
			light->BindShadowMap(counter);
			counter++;
		}
	}*/
}
