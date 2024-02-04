#version 450

out vec4 FragColor; //The color of this fragment

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
	//FragColor = texture(screenTexture, TexCoords);
	//Invert Colors
	FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
}