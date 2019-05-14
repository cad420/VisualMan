
#ifndef _SHADERPROGRAM_H_
#define _SHADERPROGRAM_H_
#include "../../lib/gl3w/GL/glcorearb.h"
#include <string>
#include <vector>
#include <map>
#include "abstrarenderstate.h"
#include "graphictype.h"

namespace ysl
{
	namespace gpu
	{
		class UniformSet;

		enum GLSLShaderType 
		{
			ST_VERTEX_SHADER = GL_VERTEX_SHADER, 
			ST_TESS_CONTROL_SHADER = GL_TESS_CONTROL_SHADER, 
			ST_TESS_EVALUATION_SHADER = GL_TESS_EVALUATION_SHADER,
			ST_GEOMETRY_SHADER = GL_GEOMETRY_SHADER, 
			ST_FRAGMENT_SHADER = GL_FRAGMENT_SHADER 
		};

		class GRAPHICS_EXPORT_IMPORT GLSLShader
		{
		public:
			GLSLShader() = delete;
			GLSLShader(GLSLShaderType type) :type(type){}
			void SetFromSource(const std::string & source);
			void SetFromFile(const std::string & fileName);
			std::string Source()const { return source; }
			void CreateShader();
			bool Compile();
			void DestroyShader();
			GLSLShaderType Type()const { return type; }
			virtual ~GLSLShader() = default;
		private:
			GLSLShaderType type;
			std::string source;
			unsigned int handle = 0;
			bool compiled = false;
		};


		class GRAPHICS_EXPORT_IMPORT GLSLVertexShader:public GLSLShader
		{
		public:
			GLSLVertexShader():GLSLShader(ST_VERTEX_SHADER){}
		};

		class GRAPHICS_EXPORT_IMPORT GLSLFragmentShader :public GLSLShader
		{
		public:
			GLSLFragmentShader() :GLSLShader(ST_FRAGMENT_SHADER) {}
		};

		class GRAPHICS_EXPORT_IMPORT GLSLTessControlShader :public GLSLShader
		{
		public:
			GLSLTessControlShader() :GLSLShader(ST_TESS_CONTROL_SHADER) {}
		};

		class GRAPHICS_EXPORT_IMPORT GLSLTessEvaluationShader :public GLSLShader
		{
		public:
			GLSLTessEvaluationShader() :GLSLShader(ST_TESS_EVALUATION_SHADER) {}
		};

		class GRAPHICS_EXPORT_IMPORT GLSLGeometryShader :public GLSLShader
		{
		public:
			GLSLGeometryShader() :GLSLShader(ST_GEOMETRY_SHADER) {}
		};

		class GRAPHICS_EXPORT_IMPORT GLSLProgram:public RenderStateNonIndexed
		{
		public:
			GLSLProgram():RenderStateNonIndexed(RS_GLSLProgram){}
			void CreateProgram();
			void DestroyProgram();
			void Link();
			bool Linked();
			void AttachShader(Ref<GLSLShader> shader);
			void DetachShader(Ref<GLSLShader> shader);
			void DetachAllShaders();
			void ApplyUniformSet(Ref<UniformSet> set);

			// RenderStateNonIndexed

			void Apply(int index, Ref<Camera> camera, RenderContext * context)const override;
		private:
			std::vector<Ref<GLSLShader>> shaders;
			std::map<std::string, int> dataLocation;
			Ref<UniformSet> uniformSet;
		};
	}
}
#endif