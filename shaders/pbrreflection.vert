#version 400
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texture_coordinates;
layout(location = 2) in vec3 normal;

uniform mat4 model, view, projection;

out vec2 TextureCoordinates;
out vec3 Position, Normal, R, V;
uniform vec3 camera_position;

void main()
{
	

	vec4 position_h = vec4(position, 1.0);
	TextureCoordinates = texture_coordinates;

	// Define the vertex colour
	Position = vec3(model * position_h);
	Normal = mat3(transpose(inverse(model))) * -normal;

	// Define the vertex position
	gl_Position = projection * view * model * position_h;

}