// author: Joe Riemersma



#version 400

out vec4 outputColor;
in vec2 TextureCoordinates;
in vec3 Position, Normal;

uniform vec3 camera_position;


uniform samplerCube reflection_cube;





void main()
{
	

	vec3 N = normalize(Normal);//normalize(Normal);
	vec3 V = normalize(Position - camera_position);

	
	vec4 reflection = texture(reflection_cube, reflect(V,N));


	outputColor = reflection;//vec4(colour,1.0);
}


