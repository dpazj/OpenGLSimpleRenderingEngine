/* 
Modified from tiny_loader.cpp
Joe Riemersma
*/



#include <glload/gl_4_0.h>

#include "ModelMesh.h"
#include <iostream>



void ModelMesh::LoadObject(const std::string& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	ProcessNode(scene->mRootNode, scene);

	SetVertexPoints(m_positions);
	SetTextureCords(m_texture_cords);
	SetNormals(m_normals);
	SetIndices(m_indices);
}

void ModelMesh::ProcessNode(aiNode* node, const aiScene* scene)
{
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		ProcessMesh(mesh, scene);
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

void ModelMesh::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		// positions
		m_positions.push_back(mesh->mVertices[i].x);
		m_positions.push_back(mesh->mVertices[i].y);
		m_positions.push_back(mesh->mVertices[i].z);

		// normals
		m_normals.push_back(mesh->mNormals[i].x);
		m_normals.push_back(mesh->mNormals[i].y);
		m_normals.push_back(mesh->mNormals[i].z);

		// texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			m_texture_cords.push_back(mesh->mTextureCoords[0][i].x);
			m_texture_cords.push_back(mesh->mTextureCoords[0][i].y);
		}
		else
		{
			m_texture_cords.push_back(0.0f);
			m_texture_cords.push_back(0.0f);
		}
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			m_indices.push_back(face.mIndices[j]);
	}
}

