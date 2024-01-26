#version 450
out vec4 FragColor; //The color of this fragment

in Surface{
	vec3 WorldPos;		//Vertex position in world space
	vec3 WorldNormal;	//Vertex normal in world space
	vec2 TexCoord;
}fs_in;

uniform sampler2D _MainTex; //2D Texture Sampler

//Lighting Uniforms
uniform vec3 _EyePos;
uniform vec3 _LightDirection = vec3(0.0,-1.0,0.0);
uniform vec3 _LightColor = vec3(1.0);
uniform vec3 _AmbientColor = vec3(0.3,0.4,0.46);



void main(){
	
	vec3 normal = normalize(fs_in.WorldNormal);
	vec3 toLight = -_LightDirection;
	float diffuseFactor = max(dot(normal, toLight),0.0);
	vec3 toEye = normalize(_EyePos - fs_in.WorldPos);
	vec3 halfAngle = normalize(toLight + toEye);
	float specularFactor = pow(max(dot(normal,halfAngle),0.0),128);
	vec3 lightColor = _LightColor * (diffuseFactor + specularFactor);
	lightColor += _AmbientColor;
	vec3 objectColor = texture(_MainTex, fs_in.TexCoord).rgb;
	FragColor = vec4(objectColor * lightColor, 1.0);
}