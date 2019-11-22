// author: Joe Riemersma



#version 400


out vec4 outputColor;
in vec2 TextureCoordinates;
in vec3 Position, Normal, R, V;

uniform vec3 camera_position;


uniform samplerCube reflection_cube;


void main()
{
	
//calculate reflection
	vec3 I = normalize(Position.xyz - camera_position);
	vec3 R = reflect(I, normalize(Normal.xyz));
	outputColor = vec4(texture(reflection_cube,R).rgb,1); 
}


