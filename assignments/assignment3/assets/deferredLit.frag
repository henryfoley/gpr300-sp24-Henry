#version 450 

out vec4 FragColor;
in vec2 TexCoords;	//From deferredLit.vert

uniform layout(binding = 0) sampler2D gPosition;
uniform layout(binding = 1) sampler2D gNormal;
uniform layout(binding = 2) sampler2D gAlbedo;

//Lighting Uniforms
uniform vec3 _EyePos;
uniform vec3 _LightDirection = vec3(0.0,-1.0,0.0);
uniform vec3 _LightColor = vec3(1.0);
uniform vec3 _AmbientColor = vec3(0.3,0.4,0.46);

struct Material{
	float Ka; //Ambeint Coefficient (0-1)
	float Kd; //Diffuse Coefficient (0-1)
	float Ks; //Specular Coefficient (0-1)
	float Shininess; //Affects size of specular highlight

};
uniform Material _Material;

void main()
{
	vec3 worldPos = texture(gPosition, TexCoords).rgb;
	vec3 normal = texture(gNormal, TexCoords).rgb;
	vec3 albedo = texture(gAlbedo, TexCoords).rgb;

	//Worldspace Lighting Calculations
	vec3 lightColor = calculateLighting(normal,worldPos,albedo);
	FragColor = vec4(albedo * lightColor, 1.0);

}