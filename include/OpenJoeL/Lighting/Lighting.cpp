////Joe Riemersma
//
//#include "Lighting.h"
//#include <string>
//#include <iostream>
//Lighting::Lighting()
//{
//
//}
//
//void Lighting::AddLightSource(std::shared_ptr<LightSource> light_source)
//{
//	if (m_lights.size() > MAX_LIGHTS) { return; }
//	m_lights.push_back(light_source);
//}
//
//std::vector < std::shared_ptr<LightSource>> Lighting::GetLightSources()
//{
//	return m_lights;
//}
//
//void Lighting::UpdateShader(std::shared_ptr<Shader> shader)
//{
//	int counter = 0;
//	shader->UseShader();
//	for (const auto& light : m_lights)
//	{
//		const std::string name_index("lights[" + std::to_string(counter) + "].");
//
//		shader->SetVec4((name_index + "position").c_str(), light->GetLightPosition());
//		shader->SetVec3((name_index + "colour").c_str(), light->GetLightColour());
//		shader->SetFloat((name_index + "constant").c_str(), light->GetConstant());
//		shader->SetFloat((name_index + "linear").c_str(), light->GetLinear());
//		shader->SetFloat((name_index + "quadratic").c_str(), light->GetQuadratic());
//		counter++;
//	}
//	shader->SetUInt("number_of_lights", (GLuint) m_lights.size());
//}