#include "widget.h"
#include "../../imgui/imgui.h"
#include "GL/gl3w.h"


void ShowGLInformation()
{
	ImGui::Begin("OpenGL Information Window");
	GLint min, max;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
	ImGui::Text("GL_MAX_TEXTURE_SIZE: %d", max);		// 32768

	glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &max);
	ImGui::Text("GL_MAX_3D_TEXTURE_SIZE:%d", max);
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max);
	ImGui::Text("GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS: %d", max);		//192

	glGetIntegerv(GL_MIN_PROGRAM_TEXEL_OFFSET, &min);
	ImGui::Text("GL_MIN_PROGRAM_TEXEL_OFFSET:%d", min);
	glGetIntegerv(GL_MAX_PROGRAM_TEXEL_OFFSET, &max);
	ImGui::Text("GL_MAX_PROGRAM_TEXEL_OFFSET:%d", max);

	glGetIntegerv(GL_MAX_FRAGMENT_IMAGE_UNIFORMS, &max);
	ImGui::Text("GL_MAX_FRAGMENT_IMAGE_UNIFORMS:%d", max);

	glGetIntegerv(GL_MAX_IMAGE_UNITS, &max);
	ImGui::Text("GL_MAX_IMAGE_UNITS:%d", max);

	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max);
	ImGui::Text("GL_MAX_VERTEX_ATTRIBS:%d", max);



	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGui::End();
}
