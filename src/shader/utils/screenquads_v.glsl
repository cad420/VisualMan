#version 420
//#extension GL_ARB_explicit_attrib_location : enable
layout( location = 0 ) in vec2 vertex;
uniform mat4 orthoMatrix;
uniform mat4 viewMatrix;
void main()
{
	gl_Position = orthoMatrix * vec4( vertex.x, vertex.y, 0.0, 1.0 );
}