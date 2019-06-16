#ifndef _RENDERSTATE_H_
#define _RENDERSTATE_H_
#include "abstrarenderstate.h"
#include "bufferobject.h"


namespace ysl
{
	namespace vm
	{
		class Texture;

		class VISUALMAN_EXPORT_IMPORT DepthFuncState :public RenderStateNonIndexed
		{
		public:
			DepthFuncState(Function fun) :RenderStateNonIndexed(RS_DepthFunc), func(fun) {}
			void Apply(int index, const Camera * camera, RenderContext * context)const override;
			void Set(Function fun) { func = fun; }
			Function DepthFunc()const { return func; }
		private:
			Function func = FU_LESS;
		};

		class VISUALMAN_EXPORT_IMPORT AtomicCounter:public RenderStateIndexed
		{
		public:
			AtomicCounter() :RenderStateIndexed(RS_AtomicCounterBuffer){}
			AtomicCounter(const std::string & name) :RenderStateIndexed(RS_AtomicCounterBuffer),atomicName(name){}
			void Apply(int index, const Camera* camera, RenderContext* context) const override;
			void SetBufferObject(Ref<BufferObject> buffer) { bufferObject = std::move(buffer); }
			Ref<const BufferObject> GetBufferObject()const { return bufferObject; }
			Ref<BufferObject> GetBufferObject() { return bufferObject; }
			std::string GetAtomicCounterName()const { return atomicName; }
			void SetAtomicCounterName(const std::string & name) { atomicName = name; }
		private:
			Ref<BufferObject> bufferObject;
			std::string atomicName;
		};

		class VISUALMAN_EXPORT_IMPORT ShaderStorageBufferObject :public RenderStateIndexed
		{
		public:
			ShaderStorageBufferObject() :RenderStateIndexed(RS_ShaderStorageBuffer) {}
			ShaderStorageBufferObject(const std::string & name) :RenderStateIndexed(RS_ShaderStorageBuffer), storageBufferName(name) {}
			void Apply(int index, const Camera* camera, RenderContext* context) const override;
			void SetBufferObject(Ref<BufferObject> buffer) { bufferObject = std::move(buffer); }
			Ref<const BufferObject> GetBufferObject()const { return bufferObject; }
			Ref<BufferObject> GetBufferObject() { return bufferObject; }
			std::string GetShaderStorageBufferName()const { return storageBufferName; }
			void SetShaderStorageBufferName(const std::string & name) { storageBufferName = name; }
		private:
			Ref<BufferObject> bufferObject;
			std::string storageBufferName;
			//unsigned int offset = 0;		// reserved field
		};

		class VISUALMAN_EXPORT_IMPORT TextureSampler :public RenderStateIndexed

		{
		public:
			TextureSampler() :RenderStateIndexed(RS_TextureSampler) {}
			void Apply(int index, const Camera * camera, RenderContext* context) const override;
			void SetTexture(Ref<Texture> texture) { this->texture = std::move(texture); }
			//void SetImageUnitName(const std::string & name) { imageUnitName = name; }
			//std::string GetImageUnitName()const { return imageUnitName; }
			Ref<Texture> GetTexture() { return texture; }
			Ref<const Texture> GetTexture()const { return texture; }
		private:
			Ref<Texture> texture;
			//std::string imageUnitName;
			//ImageUnitInfo unitInfo;
		};

		class VISUALMAN_EXPORT_IMPORT TextureImageUnit:public RenderStateIndexed
		{
		public:
			struct ImageUnitInfo
			{
				std::string unitName;
				int level = 0;
				bool layered = false;
				int layer = 0;
				ImageUnitAccess access = VM_IA_READ_WRITE;
			};
			TextureImageUnit():RenderStateIndexed(RS_TextureImageUnit){}
			void Apply(int index, const Camera* camera, RenderContext* context) const override;
			void SetTexture(Ref<Texture> texture) { this->texture = std::move(texture); }
			Ref<Texture> GetTexture() { return texture; }
			Ref<const Texture> GetTexture()const { return texture; }
		private:
			Ref<Texture> texture;
			//std::string imageUnitName;
		};

		class VISUALMAN_EXPORT_IMPORT BlendFuncState :public RenderStateNonIndexed
		{
		public:
			BlendFuncState(BlendFactor rgbSrc,
				BlendFactor alphaSrc,
				BlendFactor rgbDest,
				BlendFactor alphaDest) 
			:RenderStateNonIndexed(RS_BlendFunc), rgbSrc(rgbSrc),alphaSrc(alphaSrc),rgbDest(rgbDest),alphaDest(alphaDest)
			{
				
			}
			BlendFuncState():RenderStateNonIndexed(RS_BlendFunc){}
			void Apply(int index, const Camera* camera, RenderContext* context) const override;
			void SetRGBSrcFunc(BlendFactor fun) { rgbSrc = fun; }
			BlendFactor GetRGBSrcFunc()const{return rgbSrc;}
			void SetAlphaSrcFunc(BlendFactor fun) { alphaSrc = fun; }
			BlendFactor GetAlphaSrcFunc()const { return alphaSrc; }
			void SetRGBDestFunc(BlendFactor fun) { rgbDest = fun; }
			BlendFactor GetRGBDestFunc()const { return rgbDest; }
			void SetAlphaDestFunc(BlendFactor fun) { alphaDest = fun; }
			BlendFactor GetAlphaDestFunc()const { return alphaDest; }

		private:
			BlendFactor rgbSrc = BF_ONE;
			BlendFactor alphaSrc = BF_ONE;
			BlendFactor rgbDest = BF_ZERO;
			BlendFactor alphaDest = BF_ZERO;
		};

		class VISUALMAN_EXPORT_IMPORT BlendEquationState :public RenderStateNonIndexed
		{
		public:
			BlendEquationState():RenderStateNonIndexed(RS_BlendEquation){}
			BlendEquationState(BlendEquation rgb, BlendEquation alpha) :
				RenderStateNonIndexed(RS_BlendEquation), rgbEquation(rgb), alphaEquation(alpha) {}
			void Apply(int index, const Camera* camera, RenderContext* context) const override;

			void SetRgbEquation(BlendEquation rgb) { rgbEquation = rgb; }
			BlendEquation GetRgbEquation()const { return rgbEquation; }

			void SetAlphaEquation(BlendEquation alpha) { alphaEquation = alpha; }
			BlendEquation GetAlphaEquation()const { return alphaEquation; }

		private:
			BlendEquation rgbEquation = BE_FUNC_ADD;
			BlendEquation alphaEquation = BE_FUNC_ADD;
		};

		class VISUALMAN_EXPORT_IMPORT CullFaceState :public RenderStateNonIndexed
		{
		public:
			CullFaceState():RenderStateNonIndexed(RS_CullFace){}
			CullFaceState(PolygonFace face) :RenderStateNonIndexed(RS_CullFace), face(face) {}
			void Apply(int index, const Camera* camera, RenderContext* context) const override;
			void SetCullFace(PolygonFace face) { this->face = face; }
			PolygonFace GetCullFace()const { return this->face; }
		private:
			PolygonFace face = PF_BACK;
		};

		class VISUALMAN_EXPORT_IMPORT FrontFaceState :public RenderStateNonIndexed
		{
		public:
			FrontFaceState():RenderStateNonIndexed(RS_FrontFace) {}
			FrontFaceState(FrontFace face) :RenderStateNonIndexed(RS_FrontFace), face(face) {}
			void Apply(int index, const Camera* camera, RenderContext* context) const override;
			void SetFrontFace(FrontFace face) { this->face = face; }
			FrontFace GetFrontFace()const { return this->face; }
		private:
			FrontFace face = FF_CCW;
		};

		class VISUALMAN_EXPORT_IMPORT LineWidthState :public RenderStateNonIndexed
		{
		public:
			LineWidthState(float width) :RenderStateNonIndexed(RS_LineWidth), lineWidth(width) {}
			void Apply(int index, const Camera* camera, RenderContext* context) const override;
			void SetWidth(int width) { lineWidth = width; }
			float GetWidth()const { return lineWidth; }
		private:
			float lineWidth = 1.0;
		};




		class VISUALMAN_EXPORT_IMPORT PolygonModeState :public RenderStateNonIndexed
		{
		public:
			PolygonModeState():RenderStateNonIndexed(RS_PolygonMode){}
			PolygonModeState(PolygonMode front, PolygonMode back) :RenderStateNonIndexed(RS_PolygonMode), frontMode(front), backMode(back) {}
			void Apply(int index, const Camera* camera, RenderContext* context) const override;
			void SetFrontMode(PolygonMode mode) { frontMode = mode; }
			PolygonMode GetFrontMode()const { return backMode; }

			void SetBackMode(PolygonMode mode) { backMode = mode; }
			PolygonMode GetBackMode()const { return backMode; }

		private:
			PolygonMode frontMode = PM_FILL;
			PolygonMode backMode = PM_FILL;

		};

		//class GRAPHICS_EXPORT_IMPORT StencilOpState:public RenderStateNonIndexed
		//{
		//public:
		//	StencilOpState():RenderStateNonIndexed(RS_StencilOp){}
		//	void Apply(int index, const Camera* camera, RenderContext* context) const override;
		//	void SetOp(StencilOperator opr) { op = opr; }
		//	StencilOperator GetOp()const { return op; }
		//private:
		//	StencilOperator op = SO_KEEP;
		//};

		//class GRAPHICS_EXPORT_IMPORT StencilFuncState :public RenderStateNonIndexed
		//{
		//public:
		//	StencilFuncState(Function fun) :RenderStateNonIndexed(RS_StencilFunc), func(fun) {}
		//	void Apply(int index, const Camera * camera, RenderContext * context)const override;
		//	void Set(Function fun) { func = fun; }
		//	Function StencilFunc()const { return func; }
		//private:
		//	Function func;
		//};

		//class GRAPHICS_EXPORT_IMPORT StencilMaskState:public RenderStateNonIndexed
		//{
		//public:
		//	StencilMaskState():RenderStateNonIndexed(RS_StencilMask){}
		//	void Apply(int index, const Camera* camera, RenderContext* context) const override;

		//private:
		//	unsigned int frontMask;
		//	unsigned int backMask;
		//};



	}
}

#endif