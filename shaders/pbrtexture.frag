// author: Joe Riemersma
// Modified but based on the https://learnopengl.com/PBR/Lighting tutorial
// http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html used for the formulas 



#version 400
#define MAXLIGHTS 10

struct Light{
	vec3 position;
	vec3 colour;
};


out vec4 outputColor;
in vec2 TextureCoordinates;
in vec3 Position, Normal;

uniform vec3 camera_position;


uniform sampler2D albedo_map;
uniform sampler2D normal_map;
uniform sampler2D metallic_map;
uniform sampler2D roughness_map;
uniform sampler2D ambient_occlusion_map;


// lights
uniform Light lights[MAXLIGHTS];
uniform uint number_of_lights;

const float PI = 3.14159265359;



vec3 GetNormalFromMap()
{
    vec3 tangentNormal = texture(normal_map, TextureCoordinates).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(Position);
    vec3 Q2  = dFdy(Position);
    vec2 st1 = dFdx(TextureCoordinates);
    vec2 st2 = dFdy(TextureCoordinates);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}


vec3 calculate_fresnel(float cosT, vec3 F0) // Fresnel-Schlick approximation
{
	return F0 + (1.0 - F0) * pow(1.0 - cosT, 5.0);
}

//http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html 
float GGX(float nv, float roughness)//Schlick-Beckmann
{ 
	float r = roughness;
	float alpha = r * r;
	float k = alpha / 2;//Schlick-GGX approximation

	float denom = (nv *(1-k)) + k;
	return nv / denom;
}
//http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html 
float calculate_geometry(vec3 N, vec3 V, vec3 L, float roughness) //smith
{
	float n_dot_v = max(dot(N,V),0.0);
	float n_dot_l = max(dot(N,L),0.0);

	float g1 = GGX(n_dot_v,roughness);
	float g2 = GGX(n_dot_l,roughness);

	return g2 * g1;
}

//http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
float calculate_distribution(vec3 N, vec3 Half, float roughness) //Trowbridge-Reitz GGX
{
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
	float n_dot_h = max(dot(N,Half),0.0);
	
	float denom = ((n_dot_h * n_dot_h) * (alpha2 - 1.0)) + 1.0;
	denom = PI * denom * denom;
	return alpha2 / max(denom,0.0000001);
}

void main()
{
	vec3 albedo  = pow(texture(albedo_map, TextureCoordinates).rgb, vec3(2.2));
    float metallic = texture(metallic_map, TextureCoordinates).r;
    float roughness = texture(roughness_map, TextureCoordinates).r;
    float ambient_occlusion = texture(ambient_occlusion_map, TextureCoordinates).r;

	vec3 N = GetNormalFromMap();//normalize(Normal);
	vec3 V = normalize(camera_position - Position);

	vec3 F0 = vec3(0.04); //non metalic surface this value is always 0.04
	F0 = mix(F0, albedo, metallic); //depeneding "metallicness" on the surface we adjust this value

	vec3 LightOutput = vec3(0.0f);

	for(int i = 0; i < number_of_lights; i++)
	{
		vec3 LightDir = normalize(lights[i].position - Position);
		vec3 Half = normalize(V + LightDir);

		float distance_to_light = length(lights[i].position - Position);
		float attenuation = 1.0 / (distance_to_light * distance_to_light); //turn this into the quadratic 
		vec3 radiance = lights[i].colour * attenuation;

		vec3 Fresnel = calculate_fresnel(max(dot(Half,V),0.0),F0);
		float distribution = calculate_distribution(N,Half,roughness);
		float geometry = calculate_geometry(N,V,LightDir,roughness);

		vec3 DFG = distribution * geometry * Fresnel;
		float denom = 4 * ( max(dot(N, V),0.0) * max(dot(N, LightDir),0.0));
		vec3 Specular = DFG / max(denom,0.001);

		vec3 KS = Fresnel;
		vec3 KD = vec3(1.0) - KS;
		KD *= 1.0 - metallic;

		float n_dot_l = max(dot(N,LightDir),0.0);
		LightOutput += (KD * albedo / PI + Specular) * radiance * n_dot_l;	
	}

	vec3 ambient = vec3(0.03) * albedo * ambient_occlusion;
	vec3 colour = ambient + LightOutput;
	
	//Gamma and HDR
	colour = colour / (colour + vec3(1.0));
	colour = pow(colour, vec3(1.0/2.2)); 


	outputColor = vec4(colour,1.0);
}


