#version 450

out vec4 FragColor; //The color of this fragment

in vec2 TexCoords;

const float offset = 1.0/300.0;


uniform bool _InverseOn;
uniform int _EffectNumber;
uniform vec3 _KernelTop;
uniform vec3 _KernelCenter;
uniform vec3 _KernelBottom;
uniform float _BlurAmount;
uniform sampler2D _ScreenTexture;
uniform sampler2D _ColorBuffer;
uniform sampler2D _DepthBuffer;

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
	
	float kernel[9] = float[](
		_KernelTop.x,	_KernelTop.y,	_KernelTop.z,
		_KernelCenter.x,	_KernelCenter.y,	_KernelCenter.z,
		_KernelBottom.x,	_KernelBottom.y,	_KernelBottom.z
	);


	// Kernel Effects
	if(_EffectNumber == 0){
		vec3 sampleTex[9];
		for(int i = 0; i < 9; i++)
		{
			sampleTex[i] = vec3(texture(_ScreenTexture, TexCoords.st + offsets[i]));
		}
		vec3 col = vec3(0.0);
		for(int i = 0; i < 9; i++)
		{
			col += sampleTex[i] * kernel[i];
		}

		FragColor = vec4(col, 1.0);
	}

	// Box Blur
	else if(_EffectNumber == 1){
		vec2 texelSize = _BlurAmount/textureSize(_ScreenTexture,0).xy;		// Where blur value is being adjusted
		vec3 totalColor = vec3(0);

		for(int y = -2; y <=2; y++){
			for(int x = -2; x <=2; x++){
				vec2 offset = vec2(x,y) * texelSize;
				totalColor += texture(_ScreenTexture, TexCoords + offset).rgb;
			}
		}
		totalColor /= (5*5);

		FragColor = vec4(totalColor,1.0);
	}

	//Gamma Correction
	else if(_EffectNumber == 2){
		vec3 color = texture(_ScreenTexture,TexCoords).rgb;					// Don't think this is working properly
		color = pow(color,vec3(1.0/2.2));
		FragColor = vec4(color, 1.0);
	}

	//sRGB Correction
	else if(_EffectNumber == 3){
		vec3 diffuseColor = texture(_ScreenTexture, TexCoords).rgb;			// Don't think this is working properly
		diffuseColor = pow(diffuseColor, vec3(2.2));
		FragColor = vec4(diffuseColor, 1.0);
	}

	// Inverse Color
	if(_InverseOn){
		FragColor = vec4(vec3(1.0 -FragColor), 1.0);
	}
}