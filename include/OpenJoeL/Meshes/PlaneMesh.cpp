//Joe Riemersma

#include "PlaneMesh.h"


void PlaneMesh::Init(GLfloat length, GLfloat width)
{
	std::vector<GLfloat> vertex_positions =
	{
		-1, 0, -1,
		1,0, -1,
		1, 0, 1,

		1, 0, 1,
		-1, 0, 1,
		-1, 0, -1,


	};

	std::vector<GLfloat> texture_cords =
	{
		0, 1.f,
		0, 0,
		1.f, 0,
		1.f, 0,
		1.f,1.f,
		0, 1.f,
	};

	
	std::vector<GLfloat> normals =
	{ 
		-0, 1.f, 0,
		0, 1.f, 0,
		0, 1.f, 0,
		0, 1.f, 0,
		0, 1.f, 0,
		0, 1.f, 0,
	};

	SetVertexPoints(vertex_positions);
	SetTextureCords(texture_cords);
	SetNormals(normals);
}

