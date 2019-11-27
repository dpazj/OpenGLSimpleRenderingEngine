#pragma once
#include "Mesh.h"

class SphereMesh : public Mesh 
{
public:
	void Init(GLuint lats = 200, GLuint longs = 200);
};

