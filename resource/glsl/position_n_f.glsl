#version 430 core
in vec3 neuralTexCoord;
//Debug hxy
layout(location = 2) out vec4 neuralPos;
void main()
{
	neuralPos = vec4( neuralTexCoord, 1.0 );
	//neuralPos = vec4( 1,0,0, 1.0 );
	//return;
}