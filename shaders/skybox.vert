// Joe Riemersma

#version 420

// These are the vertex attributes
layout(location = 0) in vec3 position;


// Uniform variables are passed in from the application
uniform mat4 view, projection;


// Output the vertex colour - to be rasterized into pixel fragments
out vec3 texture_cords;



void main()
{

	texture_cords = position;
	vec4 pos = projection * view * vec4(position,1);
	gl_Position = pos.xyww; 
}



