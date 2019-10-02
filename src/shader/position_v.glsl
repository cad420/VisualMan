#version 410
#extension GL_ARB_explicit_attrib_location : enable
layout( location = 0 ) in vec4 vertex;
layout( location = 1 ) in vec3 tex;
out vec4 texCoord;
uniform mat4 projMatrix;
uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
void main()
{
	texCoord = vec4( tex, 1.0 );
	gl_Position = projMatrix * viewMatrix * worldMatrix * vertex;
}