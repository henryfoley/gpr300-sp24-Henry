#version 450

out vec4 FragColor; //The color of this fragment

in vec2 TexCoords;

uniform sampler2D _ScreenTexture;
uniform bool _InverseOn;

void main()
{
	vec2 texelSize = 1.0/textureSize(_ScreenTexture,0).xy;
	vec3 totalColor = vec3(0);

	for(int y = -2; y <=2; y++){
		for(int x = -2; x <=2; x++){
			vec2 offset = vec2(x,y) * texelSize;
			totalColor += texture(_ScreenTexture, TexCoords + offset).rgb;
		}
	}
	totalColor /= (5*5);

	FragColor = vec4(totalColor,1.0);

	//FragColor = texture(_ScreenTexture, TexCoords);

	if(_InverseOn){
		FragColor = vec4(vec3(1.0 - texture(_ScreenTexture, TexCoords)), 1.0);
	}
}