/*
	https://github.com/BennyQBD/ModernOpenGLTutorial/blob/master/obj_loader.cpp
	Modified (slightly) by Joe Riemersma
*/

#pragma once
#include <glm/glm.hpp>
#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include <string>
#include <vector>


class ModelMesh : public Mesh
{
public:
	void LoadObject(const std::string& path);

private:
	std::vector<GLfloat> m_positions;
	std::vector<GLfloat> m_texture_cords;
	std::vector<GLfloat> m_normals;
	std::vector<GLuint> m_indices;

	void ProcessNode(aiNode* node, const aiScene* scene);
	void ProcessMesh(aiMesh* mesh, const aiScene* scene);
};


