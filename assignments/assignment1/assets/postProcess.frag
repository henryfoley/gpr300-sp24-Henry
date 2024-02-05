#version 450

out vec4 FragColor; //The color of this fragment

in vec2 TexCoords;

const float offset = 1.0/300.0;

uniform float _BlurAmount;
uniform sampler2D _ScreenTexture;
uniform bool _InverseOn;

void main()
{
	vec2 offsets[9] = vec2[](
		vec2(-offset,offset),	// top	  - left
		vec2( 0.0f,	 offset),	// top	  - center
		vec2(-offset,offset),	// top	  - right
		vec2(-offset,0.0f),		// center - left
		vec2( 0.0f,	 0.0f),		// center - center
		vec2(-offset,0.0f),		// center - right
		vec2(-offset,offset),	// bottom - left
		vec2( 0.0f,	 offset),	// bottom - center
		vec2(-offset,offset)	// bottom - right
	);

	float kernal[9] = float[](
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16  
	);

	// Kernal Effects
	vec3 sampleTex[9];
	for(int i = 0; i < 9; i++)
	{
		sampleTex[i] = vec3(texture(_ScreenTexture, TexCoords.st + offsets[i]));
	}
	vec3 col = vec3(0.0);
	for(int i = 0; i < 9; i++)
	{
		col += sampleTex[i] * kernal[i];
	}

	FragColor = vec4(col, 1.0);

	// Box Blur
	//vec2 texelSize = _BlurAmount/textureSize(_ScreenTexture,0).xy;		// Where blur value is being adjusted
	//vec3 totalColor = vec3(0);

	//for(int y = -2; y <=2; y++){
		//for(int x = -2; x <=2; x++){
			//vec2 offset = vec2(x,y) * texelSize;
			//totalColor += texture(_ScreenTexture, TexCoords + offset).rgb;
		//}
	//}
	//totalColor /= (5*5);

	//FragColor = vec4(totalColor,1.0);

	// Inverse Color
	if(_InverseOn){
		FragColor = vec4(vec3(1.0 -FragColor), 1.0);
	}
}