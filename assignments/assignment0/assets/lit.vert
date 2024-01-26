#version 450
//Vertex attributes
layout(location = 0) in vec3 vPos; //Vertex position in model space
layout(location = 1) in vec3 vNormal; //Vertex position in model space
layout(location = 2) in vec2 vTexCoord; //Vertex texture coordinate
layout(location = 3) in vec3 vTangent;
layout(location = 4) in vec3 vBitangent;

uniform mat4 _Model; //Model->World Matrix
uniform mat4 _ViewProjection; //Combined View->Projection Matrix

out Surface{
	vec2 TexCoord;
	vec3 WorldPos;		//Vertex position in world space
	vec3 WorldNormal;	//Vertex normal in world space
	mat3 TBN;
}vs_out;

void main(){
	vs_out.TexCoord = vTexCoord;
	//Transform vertex position to World Space
	vs_out.WorldPos = vec3(_Model * vec4(vPos,1.0));

	//Transform vertex normal to world space using Normal Matrix
	vs_out.WorldNormal = transpose(inverse(mat3(_Model))) * vNormal;

	vec3 T = normalize(vec3 (_Model * vec4(vTangent, 0.0)));
	vec3 B = normalize(vec3 (_Model * vec4(vBitangent, 0.0)));
	vec3 N = normalize(vec3 (_Model * vec4(vNormal, 0.0)));

	mat3 TBN = mat3(T, B, N);
	vs_out.TBN = TBN;

	//Transform vertex position to homogeneous clip space
	gl_Position = _ViewProjection * _Model * vec4(vPos,1.0);
}
