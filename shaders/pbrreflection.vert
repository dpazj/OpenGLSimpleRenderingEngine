#version 400
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texture_coordinates;
layout(location = 2) in vec3 normal;

uniform mat4 model, view, projection;

out vec2 TextureCoordinates;
out vec3 Position, Normal;


void main()
{
	
	vec4 h_pos =  vec4(position,1);

	TextureCoordinates = texture_coordinates;
	Position = vec3(model * h_pos);
	Normal = mat3(transpose(inverse(model))) * normal;
	gl_Position = projection * view * model * h_pos;
}