// author: Joe Riemersma
// Modified but based on the https://learnopengl.com/PBR/Lighting tutorial
// http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html used for the formulas 



#version 400

out vec4 outputColor;
in vec2 TextureCoordinates;
in vec3 Position, Normal;
in vec4 PositionLightSpace;

uniform vec3 camera_position;
uniform sampler2D shadow_map;

uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ambient_occlusion;


// lights
uniform vec3 light_positions[4];
uniform vec3 light_colors[4];
uniform vec3 light_pos;

const float PI = 3.14159265359;



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


float calculate_shadow()
{
	vec3 projected = PositionLightSpace.xyz / PositionLightSpace.w;
	projected = projected * 0.5 + 0.5; 

	float closest = texture(shadow_map, projected.xy).r;
	float current = projected.z;

	float bias = max(0.05 * (1.0 - dot(Normal, normalize(light_positions[0] - Position))), 0.01);

    float shadow = 0.0;

	vec2 texelSize = 1.0 / textureSize(shadow_map, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadow_map, projected.xy + vec2(x, y) * texelSize).r; 
            shadow += current - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
 
    if(projected.z > 1.0)
        shadow = 0.0;
        
    return shadow;
	
}

void main()
{
	
	vec3 N = normalize(Normal);//normalize(Normal);
	vec3 V = normalize(camera_position - Position);

	vec3 F0 = vec3(0.04); //non metalic surface this value is always 0.04
	F0 = mix(F0, albedo, metallic); //depeneding "metallicness" on the surface we adjust this value

	vec3 LightOutput = vec3(0.0f);

	for(int i = 0; i < 1; i++)
	{
		vec3 LightDir = normalize(light_positions[i] - Position);
		vec3 Half = normalize(V + LightDir);

		float distance_to_light = length(light_positions[i] - Position);
		float attenuation = 1.0 / (distance_to_light * distance_to_light); //turn this into the quadratic 
		vec3 radiance = light_colors[i] * attenuation;

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

		float shadow = calculate_shadow();

		LightOutput += ((KD * albedo / PI + Specular) + (-0.5 * shadow)) * radiance * n_dot_l;	
	}


	vec3 ambient = (vec3(0.03) * albedo * (ambient_occlusion));
	vec3 colour = ambient + LightOutput;
	
	//Gamma and HDR
	colour = colour / (colour + vec3(1.0));
	colour = pow(colour, vec3(1.0/2.2)); 
	outputColor = vec4(colour,1.0);

//	vec3 color = albedo;
//    vec3 normal = normalize(Normal);
//    vec3 lightColor = vec3(0.3);
//    // ambient
//    vec3 ambient = 0.3 * color;
//    // diffuse
//    vec3 lightDir = normalize(light_positions[0] - Position);
//    float diff = max(dot(lightDir, normal), 0.0);
//    vec3 diffuse = diff * lightColor;
//    // specular
//    vec3 viewDir = normalize(camera_position - Position);
//    vec3 reflectDir = reflect(-lightDir, normal);
//    float spec = 0.0;
//    vec3 halfwayDir = normalize(lightDir + viewDir);  
//    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
//    vec3 specular = spec * lightColor;    
//    // calculate shadow
//    float shadow = calculate_shadow();                      
//    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
//
//	outputColor = vec4(lighting, 1.0);

}


