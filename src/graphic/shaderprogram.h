
#ifndef _SHADERPROGRAM_H_
#define _SHADERPROGRAM_H_

#include "../../lib/gl3w/GL/glcorearb.h"
#include <string>
#include <vector>
#include <map>
#include "abstrarenderstate.h"
#include "graphictype.h"
#include "uniform.h"

namespace ysl
{
	namespace graphics
	{
		class UniformSet;



		class GRAPHICS_EXPORT_IMPORT GLSLShader
		{
		public:
			GLSLShader() = delete;
			GLSLShader(GLSLShaderType type) :type(type) {}
			void SetFromSource(const std::string & source);
			void SetFromFile(const std::string & fileName);
			std::string Source()const { return source; }
			void CreateShader();
			bool Compile();
			void DestroyShader();
			GLSLShaderType Type()const { return type; }
			int Handle()const { return handle; }
			virtual ~GLSLShader();
		private:
			GLSLShaderType type;
			std::string source;
			unsigned int handle = 0;
			bool compiled = false;
		};


		class GRAPHICS_EXPORT_IMPORT GLSLVertexShader :public GLSLShader
		{
		public:
			GLSLVertexShader() :GLSLShader(ST_VERTEX_SHADER) {}
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

		class GRAPHICS_EXPORT_IMPORT GLSLProgram :public RenderStateNonIndexed
		{
		public:
			GLSLProgram() :RenderStateNonIndexed(RS_GLSLProgram) {}

			virtual ~GLSLProgram();
			void CreateProgram();
			void DestroyProgram();

			bool Link();
			bool Linked();
			bool Reload();
			void AttachShader(Ref<GLSLShader> shader);
			void DetachShader(Ref<GLSLShader> shader);
			void DetachAllShaders();
			void ApplyUniformSet(Ref<UniformSet> set);

			int GetUniformLocation(const char * name)const;

			int WorldMatrixLocation()const;
			int ViewMatrixLocation()const;
			int ProjectionMatrixLocation()const;
			int NormalMatrixLocation()const;


			// Vertex Attribute 

			int VertexPositionLocation()const;
			int VertexNormalLocation()const;
			int VertexTexCoordLocation()const;
			int VertexColorLocation()const;

			Ref<Uniform> GetUniform();
			Ref<Uniform> CreateUniform(const char * name);
			void RemoveUniform(const char *name);
			void RemoveUniform(const Ref<Uniform> & uniform);

			void GetAttribLocation(const char *name);
			int Handle()const { return handle; }


			// RenderStateNonIndexed

			void Apply(int index, const Camera * camera, RenderContext * context)const override;
		private:
			std::vector<Ref<GLSLShader>> shaders;
			std::map<std::string, int> dataLocation;
			Ref<UniformSet> uniformSet;
			unsigned int handle = 0;
			bool linked = false;
		};
	}
}

#endif