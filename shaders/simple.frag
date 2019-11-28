//Joe Riemersma
#version 420

#define MAX_LIGHTS 100

out vec4 outputColor;

uniform vec3 albedo;

void main()
{
	outputColor = vec4(albedo,1);
}












