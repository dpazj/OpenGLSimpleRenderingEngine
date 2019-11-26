//Joe Riemersma

#pragma once
#include "OpenJoeL/Meshes/Mesh.h"

class PlaneMesh : public Mesh
{
public:
	void Init(GLfloat length = 1, GLfloat width = 1);
};