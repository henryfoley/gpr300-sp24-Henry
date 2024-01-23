#version 450
//Vertex attributes
layout(location = 0) in vec3 vPos; //Vertex position in model space
layout(location = 1) in vec3 vNormal; //Vertex position in model space
layout(location = 2) in vec2 vTexCoord; //Vertex textue coordinate

uniform mat4 _Model; //Model->World Matrix
uniform mat4 _ViewProjection; //Combined View->Projection Matrix

out Surface{
	vec3 Normal; //Output to next shader
	vec2 TexCoord;
}vs_out;

void main(){
	vs_out.Normal = vNormal;
	vs_out.TexCoord = vTexCoord;
	//Transform vertex position to homogeneous clip space
	gl_Position = _ViewProjection * _Model * vec4(vPos,1.0);
}
