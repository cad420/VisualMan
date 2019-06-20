#version 430 core
in vec3 texCoord;
out vec4 entryPos;
out vec4 exitPos;

uniform vec3 viewPos;
uniform mat4 vpl_ModelMatrix;

//out vec4 fragColor;
void main() 
{
	vec3 maxPoint = vec3(vpl_ModelMatrix*vec4(1));
	vec3 minPoint = vec3(vpl_ModelMatrix*vec4(0,0,0,1));

	bool inner = false;
	vec3 eyePos = viewPos;
	if(eyePos.x >= minPoint.x && eyePos.x <= maxPoint.x 
	&& eyePos.y >= minPoint.y && eyePos.y <= maxPoint.y 
	&& eyePos.z >= minPoint.z && eyePos.z <= maxPoint.z)
		inner = true;

	if(gl_FrontFacing)
	{
		entryPos = vec4(texCoord,1.0);
		exitPos = vec4(0,0,0,0);
	}
	else
	{
		exitPos = vec4(texCoord,1.0);
		if(inner){
			eyePos = (eyePos-minPoint)/(maxPoint-minPoint);		// normalized to sample space of [0,1]^3
			entryPos=vec4(eyePos,0);
		}else{
			entryPos=vec4(0,0,0,0);
		}
	}
	return;
}