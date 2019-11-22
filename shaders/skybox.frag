//Joe Riemersma
#version 420

out vec4 fcolour;

in vec3 texture_cords;

uniform samplerCube skybox;

void main()
{    
    fcolour = texture(skybox, texture_cords);
}










