#version 450 core
layout(location = 0) out vec3 gPosition;	//Worldspace Position
layout(location = 1) out vec3 gNormal;		//Worldspace Normal
layout(location = 2) out vec3 gAlbedo;		//Albedo

in Surface{
	vec2 TexCoord;
	vec3 WorldPos;		//Vertex position in world space
	vec3 WorldNormal;	//Vertex normal in world space
	mat3 TBN;
}fs_in;

uniform sampler _MainTex;
uniform sampler _NormalTex;

void main(){
	gPosition = fs_in.WorldPos;

	//Normal Mapping
	gNormal = normalize(fs_in.WorldNormal);
	gNormal = texture(_NormalTex, fs_in.TexCoord).rgb;
	gNormal = gNormal * 2.0 - 1.0;
	gNormal = normalize(fs_in.TBN * gNormal);

	gAlbedo = texture(_MainTex, fs_in.TexCoord).rgb;
}