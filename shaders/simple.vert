// Joe Riemersma

#version 420

// These are the vertex attributes
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texture_cords;
layout(location = 2) in vec3 normal;

uniform mat4 projection, view, model;


void main()
{
	gl_Position = projection * view * model * vec4(position,1);
}



