#include "SphereMesh.h"


void SphereMesh::Init(GLuint lats, GLuint longs)
{
	GLuint numvertices = 2 + ((lats - 1) * longs);
	GLuint i, j;
	
	std::vector<GLfloat> positions = std::vector<GLfloat>(numvertices * 3, 0.0f);
	std::vector<GLfloat> uv = std::vector<GLfloat>(numvertices * 2, 0.0f);
	std::vector<GLfloat> normals;
	std::vector<unsigned int> indices;

	

	GLfloat DEG_TO_RADIANS = 3.141592f / 180.f;
	GLuint vnum = 0;
	GLfloat x, y, z, lat_radians, lon_radians;

	/* Define north pole */
	positions[0] = 0; positions[1] = 0; positions[2] = 1.f;
	vnum++;

	GLfloat latstep = 180.f / lats;
	GLfloat longstep = 360.f / longs;

	/* Define vertices along latitude lines */
	for (GLfloat lat = 90.f - latstep; lat > -90.f; lat -= latstep)
	{
		lat_radians = lat * DEG_TO_RADIANS;
		for (GLfloat lon = -180.f; lon < 180.f; lon += longstep)
		{
			lon_radians = lon * DEG_TO_RADIANS;

			x = cos(lat_radians) * cos(lon_radians);
			y = cos(lat_radians) * sin(lon_radians);
			z = sin(lat_radians);

			/* Define the vertex */
			positions[vnum * 3] = x; positions[vnum * 3 + 1] = y; positions[vnum * 3 + 2] = z;

			float u = (lon + 180.f) / 360.f;
			float v = (lat + 90.f) / 180.f;
			uv[vnum * 2] = u;
			uv[vnum * 2 + 1] = v;

			vnum++;
		}
	}
	/* Define south pole */
	positions[vnum * 3] = 0; positions[vnum * 3 + 1] = 0; positions[vnum * 3 + 2] = -1.f;
	normals = positions;

	for (i = 0; i < longs + 1; i++)
	{
		indices.push_back(i);
	}
	indices.push_back(1);	// Join last triangle in the triangle fan

	GLuint start = 1;		// Start index for each latitude row
	for (j = 0; j < lats - 2; j++)
	{
		for (i = 0; i < longs; i++)
		{
			indices.push_back(start + i);
			indices.push_back(start + i + longs);
		}
		indices.push_back(start); // close the triangle strip loop by going back to the first vertex in the loop
		indices.push_back(start + longs); // close the triangle strip loop by going back to the first vertex in the loop

		start += longs;
	}

	// Define indices for the last triangle fan for the south pole region
	for (i = numvertices - 1; i > (numvertices - longs - 2); i--)
	{
		indices.push_back(i);
	}
	indices.push_back(numvertices - 2);	// Tie up last triangle in fan


	SetVertexPoints(positions);
	SetTextureCords(uv);
	SetNormals(normals);
	SetIndices(indices);
	
	const auto indice_count = indices.size();

	SetCustomDraw([indice_count]() {glDrawElements(GL_TRIANGLE_STRIP, indice_count, GL_UNSIGNED_INT, 0); });
	
}
