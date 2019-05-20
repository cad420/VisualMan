
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
	namespace vpl
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
			void ApplyUniformSet(Ref<const UniformSet> set)const;


			int GetWorldMatrixUniformLocation()const;
			int GetViewMatrixUniformLocation()const;
			int GetProjectionMatrixUniformLocation()const;
			int GetNormalMatrixUniformLocation()const;
			int GetGenericUniformLocation(const char * name)const;


			// Vertex Attribute 

			int GetVertexPositionAttribLocation()const;
			int VertexNormalAttribLocation()const;
			int VertexTexCoordAttriLocation(int index)const;
			int VertexColorAttribLocation()const;
			void GetGenericAttribLocation(const char *name);

			Ref<Uniform> GetUniform(const char * name);

			Ref<Uniform> CreateGetUniform(const char * name);
			Ref<UniformSet> CreateGetUniformSet() { return uniformSet ? uniformSet : uniformSet = MakeRef<UniformSet>(); }
			Ref<const UniformSet> GetUniformSet()const { return uniformSet; }
			Ref<UniformSet> GetUniformSet() { return uniformSet; }

			void RemoveUniform(const char * name);
			void RemoveUniform(const Ref<Uniform> & uniform);

			int Handle()const { return handle; }
			// RenderStateNonIndexed

			void Apply(int index, const Camera * camera, RenderContext * context)const override;
		private:

			void PreLink();
			void PostLink();

			struct PredefinedUniformLocation
			{
				int vpl_ModelMatrix = -1;
				int vpl_ViewMatrix = -1;
				int vpl_ProjectionMatrix = -1;
				int vpl_MVPMatrix = -1;
				int vpl_NormalMatrix = -1;
			}uniformLocations;

			struct PredefinedAttributeLocation
			{
				int vpl_VertexPosition = VA_VertexPositionAttribLocation;
				int vpl_VertexColor = VA_VertexColorAttribLocation;
				int vpl_VertexNormal = VA_VertexNormalAttribLocation;
				int vpl_VertexTexCoord[5] = 
				{
					VA_VertexTexCoord0AttribLocation,
					VA_VertexTexCoord1AttribLocation,
					VA_VertexTexCoord2AttribLocation,
					VA_VertexTexCoord3AttribLocation,
					VA_VertexTexCoord4AttribLocation,
				};

			}attribLocations;



			std::vector<Ref<GLSLShader>> shaders;
			std::map<std::string, int> dataLocation;
			Ref<UniformSet> uniformSet;
			unsigned int handle = 0;
			bool linked = false;
		};
	}
}

#endif