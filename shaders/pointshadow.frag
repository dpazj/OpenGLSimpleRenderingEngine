//Joe Riemersma
#version 420

in vec4 Position;

uniform vec3 light_pos;
uniform float far_plane;

void main()
{
    float light_distance = length(Position.xyz - light_pos);
    light_distance = light_distance / far_plane;
    gl_FragDepth = light_distance;
}  









