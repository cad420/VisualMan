
#ifndef _PROGRAM_H_
#define _PROGRAM_H_
#include <string>
#include "../../lib/gl3w/GL/glcorearb.h"
#include "graphictype.h"
#include <vector>
#include <map>
#include "abstrarenderstate.h"

namespace ysl
{
	namespace gpu
	{
		class UniformSet;

		enum GLSLShaderType 
		{
			ST_VERTEX_SHADER = GL_VERTEX_SHADER, //!< A shader that is intended to run on the programmable vertex processor.
			ST_TESS_CONTROL_SHADER = GL_TESS_CONTROL_SHADER, //!< A shader that is intended to run on the programmable tessellation processor in the control stage.
			ST_TESS_EVALUATION_SHADER = GL_TESS_EVALUATION_SHADER, //!< A shader that is intended to run on the programmable tessellation processor in the evaluation stage.
			ST_GEOMETRY_SHADER = GL_GEOMETRY_SHADER, //!< A shader that is intended to run on the programmable geometry processor.
			ST_FRAGMENT_SHADER = GL_FRAGMENT_SHADER //!< A shader that is intended to run on the programmable fragment processor.
		};

		class GLSLShader
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
			unsigned int handle;
			bool compiled;
		};


		class GLSLVertexShader:public GLSLShader
		{
		public:
			GLSLVertexShader():GLSLShader(ST_VERTEX_SHADER){}
		};

		class GLSLFragmentShader :public GLSLShader
		{
		public:
			GLSLFragmentShader() :GLSLShader(ST_FRAGMENT_SHADER) {}
		};

		class GLSLTessControlShader :public GLSLShader
		{
		public:
			GLSLTessControlShader() :GLSLShader(ST_TESS_CONTROL_SHADER) {}
		};

		class GLSLTessEvaluationShader :public GLSLShader
		{
		public:
			GLSLTessEvaluationShader() :GLSLShader(ST_TESS_EVALUATION_SHADER) {}
		};

		class GLSLGeometryShader :public GLSLShader
		{
		public:
			GLSLGeometryShader() :GLSLShader(ST_GEOMETRY_SHADER) {}
		};


		class GLSLProgram:public RenderStateNonIndexed
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
			void ApplyUniformSet(Ref<UniformSet> uset);

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