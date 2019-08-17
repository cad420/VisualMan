#version 330 core
in vec3 frag_pos;
out vec4 frag_color;
uniform vec4 object_color;
void main()
{
	frag_color = object_color;
}
