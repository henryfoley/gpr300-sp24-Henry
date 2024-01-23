#version 450
out vec4 FragColor; //The color of this fragment

in Surface{
	vec3 Normal;
	vec2 TexCoord;
}fs_in;

uniform sampler2D _MainTex; //2D Texture Sampler

void main(){
	//Shade with 0-1 normal
	FragColor = texture(_MainTex, fs_in.TexCoord);
}