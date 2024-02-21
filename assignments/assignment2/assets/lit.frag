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

//Slope Bias
uniform float _MinSlopeBias;
uniform float _MaxSlopeBias;

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

float calcShadow(vec3 normal,vec3 toLight, sampler2D shadowMap, vec4 LightSpacePos){
	
	float shadow = 0.0f;
	vec3 sampleCoord = LightSpacePos.xyz / LightSpacePos.w;
	sampleCoord = sampleCoord * 0.5 + 0.5;

	//Bias
	float bias = max(_MaxSlopeBias * (1.0 - dot(normal,toLight)),_MinSlopeBias);
	
	float myDepth = sampleCoord.z - bias;
	if(myDepth >= 1.0f){
		return 0;
	}
	float shadowMapDepth = texture(shadowMap, sampleCoord.xy).r;
	shadow = step(shadowMapDepth, myDepth);

	//Percentage Closer Filtering
	vec2 texelOffset = 1.0 / textureSize(_ShadowMap, 0);
	for(int y = -1; y <= 1; y++){
		for(int x = -1; x <= 1; x++){
			float pcfDepth = texture(shadowMap, sampleCoord.xy + vec2(x * texelOffset.x, y * texelOffset.y)).r;
			shadow += step(pcfDepth, myDepth);

			//vec2 uv  = LightSpacePos.xy + vec2(x * texelOffset.x, y * texelOffset.y);
			//shadow += step(texture(_ShadowMap, uv).r, myDepth);
		}
	}
	shadow /= 9.0;

	return shadow;
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

	//Shadow Calculation
	float shadow = calcShadow(normal,toLight,_ShadowMap, LightSpacePos);

    vec3 lightColor = _LightColor * (_Material.Kd * diffuseFactor + _Material.Ks * specularFactor);
   
    lightColor *= (1.0 - shadow); // Factor in shadow
	lightColor += _AmbientColor * _Material.Ka;
	vec3 objectColor = texture(_MainTex, fs_in.TexCoord).rgb;
	FragColor = vec4(objectColor * lightColor, 1.0);
}