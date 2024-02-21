#version 450
out vec4 FragColor; //The color of this fragment

in Surface{
	vec2 TexCoord;
	vec3 WorldPos;		//Vertex position in world space
	vec3 WorldNormal;	//Vertex normal in world space
	mat3 TBN;			//TBN Matrix
}fs_in;

in vec4 LightSpacePos;

uniform sampler2D _MainTex;		//2D Texture Sampler
uniform sampler2D _NormalTex;	//Normal Sampler
uniform sampler2D _ShadowMap;	//Shadow Map Sampler

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

float calcShadow(sampler2D shadowMap, vec4 LightSpacePos){
	
	vec3 sampleCoord = LightSpacePos.xyz / LightSpacePos.w;
	sampleCoord = sampleCoord * 0.5 + 0.5;
	float myDepth = sampleCoord.z;
	if(myDepth >= 1.0f){
		return 0;
	}
	float shadowMapDepth = texture(shadowMap, sampleCoord.xy).r;
	return step(shadowMapDepth, myDepth);
}

void main(){
	
	//Normal Maps
	vec3 normal = normalize(fs_in.WorldNormal);
	normal = texture(_NormalTex, fs_in.TexCoord).rgb;
	normal = normal * 2.0 - 1.0;
	normal = normalize(fs_in.TBN * normal);

	vec3 toLight = -_LightDirection;
	float diffuseFactor = max(dot(normal, toLight),0.0);

	vec3 toEye = normalize(_EyePos - fs_in.WorldPos);

	vec3 halfAngle = normalize(toLight + toEye);

	float specularFactor = pow(max(dot(normal,halfAngle),0.0),_Material.Shininess);

	float shadow = calcShadow(_ShadowMap, LightSpacePos);

    vec3 lightColor = _LightColor * (_Material.Kd * diffuseFactor + _Material.Ks * specularFactor);
   
    lightColor *= (1.0 - shadow); // Factor in shadow
	lightColor += _AmbientColor * _Material.Ka;
	vec3 objectColor = texture(_MainTex, fs_in.TexCoord).rgb;
	FragColor = vec4(objectColor * lightColor, 1.0);
}