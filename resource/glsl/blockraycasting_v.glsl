#version 420 core
//#extension GL_ARB_explicit_attrib_location : enable
layout (location = 0 ) in vec2 vertex;
layout (location = 1 ) in vec2 tex;
out vec2 textureRectCoord;
uniform mat4 orthoMatrix;
uniform mat4 viewMatrix;
void main() {
   textureRectCoord = tex;
   gl_Position = orthoMatrix*vec4(vertex.x,vertex.y,0.0,1.0);
};