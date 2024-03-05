#version 450 

out vec4 FragColor;
in vec2 TexCoords;	//From deferredLit.vert

uniform layout(binding = 0) sampler2D _gPosition;
uniform layout(binding = 1) sampler2D _gNormal;
uniform layout(binding = 2) sampler2D _gAlbedo;

uniform sampler2D _ShadowMap;	//Shadow Map Sampler

//Slope Bias
uniform float _MinSlopeBias;
uniform float _MaxSlopeBias;

//Lighting Uniforms
uniform vec3 _EyePos;
uniform vec3 _LightDirection = vec3(0.0,-1.0,0.0);
uniform vec3 _LightColor = vec3(1.0);
uniform vec3 _AmbientColor = vec3(0.3,0.4,0.46);

//Shadow Map Uniforms
uniform mat4 _LightViewProjection; //Combined Light View->Projection Matrix

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
		}
	}
	shadow /= 9.0;

	return shadow;
}

vec3 calcLighting(vec3 normal,vec3 worldPos,vec3 albedo){

	vec3 toLight = -_LightDirection;
	float diffuseFactor = max(dot(normal, toLight),0.0);

	vec3 toEye = normalize(_EyePos - worldPos);

	vec3 halfAngle = normalize(toLight + toEye);

	float specularFactor = pow(max(dot(normal,halfAngle),0.0),_Material.Shininess);

	//LightSpacePos
	vec4 LightSpacePos = _LightViewProjection * vec4(worldPos,1.0);

	//Shadow
	float shadow = calcShadow(normal,toLight,_ShadowMap,LightSpacePos);	

	//Factor in shadow
	vec3 lightColor = _LightColor * (_Material.Kd * diffuseFactor + _Material.Ks * specularFactor);
    lightColor *= (1.0 - shadow); 
	lightColor += _AmbientColor * _Material.Ka;

	return lightColor;
}

void main()
{
	vec3 worldPos = texture(_gPosition, TexCoords).rgb;
	vec3 normal = texture(_gNormal, TexCoords).rgb;
	vec3 albedo = texture(_gAlbedo, TexCoords).rgb;

	//Worldspace Lighting Calculations
	vec3 lightColor = calcLighting(normal,worldPos,albedo);
	FragColor = vec4(albedo * lightColor, 1.0);
}