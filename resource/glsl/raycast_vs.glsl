#version 330
layout (location = 0 ) in vec2 vertex;
layout (location = 1 ) in vec2 tex;
out vec2 textureRectCoord;
uniform mat4 othoMatrix;
uniform mat4 viewMatrix;
void main() 
{
   textureRectCoord = tex;
   gl_Position = othoMatrix*vec4(vertex.x,vertex.y,0.0,1.0);
};