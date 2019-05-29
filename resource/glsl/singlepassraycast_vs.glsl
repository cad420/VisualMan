#version 330 core

out vec3 frag_position; // in object space
out vec4 tex_coord;

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

void main(void)
{
    gl_Position = vpl_MVPMatrix * vpl_VertexPosition;
    //tex_coord = vpl_VertexTexCoord0;
    frag_position = (vpl_ModelMatrix*vpl_VertexTexCoord0).xyz;
}