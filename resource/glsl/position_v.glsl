#version 430 core
uniform mat4 vpl_ModelMatrix;
uniform mat4 vpl_ViewMatrix;
uniform mat4 vpl_ProjectionMatrix;
uniform mat4 vpl_MVPMatrix;
uniform mat3 vpl_NormalMatrix;

in vec4 vpl_VertexPosition;
in vec4 vpl_VertexNormal;
in vec4 vpl_VertexColor;
in vec4 vpl_VertexTexCoord0;
in vec4 vpl_VertexTexCoord1;
in vec4 vpl_VertexTexCoord2;
in vec4 vpl_VertexTexCoord3;
in vec4 vpl_VertexTexCoord4;

out vec3 texCoord;

void main()
{
	gl_Position = vpl_MVPMatrix * vpl_VertexPosition;
	texCoord = vec3(vpl_VertexTexCoord0);            //(0,0,0)<--->(1,1,1)
}