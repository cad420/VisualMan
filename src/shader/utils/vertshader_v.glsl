
#version 330

uniform mat4 modelViewMat;
uniform mat4 projMat;
layout( location = 0 ) in vec3 vertex;
void main()
{
	gl_Position = projMat * modelViewMat * vec4( vertex, 1.0 );
};