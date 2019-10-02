#version 430

uniform sampler2DRect resTex;
in vec2 textureRectCoord;
out vec4 fragColor;
void main()
{
	fragColor = texture2DRect( resTex, textureRectCoord );
}