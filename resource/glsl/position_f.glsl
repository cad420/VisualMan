#version 430 core
in vec3 texCoord;
out vec4 entryPos;
out vec4 exitPos;
//out vec4 fragColor;
void main() 
{
	//float d = gl_FragDepth;
	//gl_FragDepth = d;
	if(gl_FrontFacing)
	{
		entryPos = vec4(texCoord,1.0);
		exitPos = vec4(0,0,0,0);
	}
	else{
		exitPos = vec4(texCoord,1.0);
		entryPos=vec4(0,0,0,0);
	}
	return;
}