#ifndef _RENDERSTATE_H_
#define _RENDERSTATE_H_
#include "abstrarenderstate.h"


namespace ysl
{
	namespace vm
	{
		class Texture;

		class GRAPHICS_EXPORT_IMPORT DepthFuncState :public RenderStateNonIndexed
		{
		public:
			DepthFuncState(Function fun) :RenderStateNonIndexed(RS_DepthFunc), func(fun) {}
			void Apply(int index, const Camera * camera, RenderContext * context)const override;
			void Set(Function fun) { func = fun; }
			Function DepthFunc()const { return func; }
		private:
			Function func;
		};


		class GRAPHICS_EXPORT_IMPORT TextureSampler :public RenderStateIndexed

		{
		public:
			TextureSampler() :RenderStateIndexed(RS_TextureSampler) {}
			void Apply(int index, const Camera * camera, RenderContext* context) const override;
			void SetTexture(Ref<Texture> texture) { this->texture = std::move(texture); }
			Ref<Texture> GetTexture() { return texture; }
			Ref<const Texture> GetTexture()const { return texture; }
		private:
			Ref<Texture> texture;
		};

		class GRAPHICS_EXPORT_IMPORT BlendFuncState :public RenderStateNonIndexed
		{
		public:
			BlendFuncState(BlendFactor rgbSrc,
				BlendFactor alphaSrc,
				BlendFactor rgbDest,
				BlendFactor alphaDest) 
			:RenderStateNonIndexed(RS_BlendFunc), rgbSrc(rgbSrc),alphaSrc(alphaSrc),rgbDest(rgbDest),alphaDest(alphaDest)
			{
				
			}
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
			BlendFactor rgbSrc = BF_SRC_COLOR;
			BlendFactor alphaSrc = BF_SRC_ALPHA;
			BlendFactor rgbDest = BF_ONE_MINUS_SRC_COLOR;
			BlendFactor alphaDest = BF_ONE_MINUS_DST_ALPHA;
		};

		class GRAPHICS_EXPORT_IMPORT BlendEquationState :public RenderStateNonIndexed
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

		class GRAPHICS_EXPORT_IMPORT CullFaceState :public RenderStateNonIndexed
		{
		public:
			CullFaceState():RenderStateNonIndexed(RS_CullFace){}
			CullFaceState(PolygonFace face) :RenderStateNonIndexed(RS_CullFace), face(face) {}
			void Apply(int index, const Camera* camera, RenderContext* context) const override;
			void SetFace(PolygonFace face) { this->face = face; }
			PolygonFace GetFace()const { return this->face; }
		private:
			PolygonFace face = PF_BACK;
		};

		class GRAPHICS_EXPORT_IMPORT LineWidthState :public RenderStateNonIndexed
		{
		public:
			LineWidthState(float width) :RenderStateNonIndexed(RS_LineWidth), lineWidth(width) {}
			void Apply(int index, const Camera* camera, RenderContext* context) const override;
			void SetWidth(int width) { lineWidth = width; }
			float GetWidth()const { return lineWidth; }
		private:
			float lineWidth = 1.0;
		};


		class GRAPHICS_EXPORT_IMPORT PolygonModeState :public RenderStateNonIndexed
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