#version 450

//Vertex attributes
layout(location = 0) in vec3 vPos; //Vertex position in model space
layout(location = 1) in vec2 vTexCoord; //Vertex texture coordinate

out vec2 TexCoords;

void main()
{
	gl_Position = vec4(vPos.x, vPos.y, 0.0, 1.0);
	TexCoords = vTexCoord;
}