#version 410
in vec4 texCoord;
out vec4 fragColor;
void main()
{
	fragColor = texCoord;
}