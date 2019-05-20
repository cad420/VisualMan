#ifndef _RENDERSTATE_H_
#define _RENDERSTATE_H_
#include "abstrarenderstate.h"


namespace ysl
{
	namespace vpl
	{
		class Texture;

		class GRAPHICS_EXPORT_IMPORT DepthFuncState:public RenderStateNonIndexed
		{
		public:
			DepthFuncState(Func fun) :RenderStateNonIndexed(RS_DepthFunc),func(fun){}
			void Apply(int index, const Camera * camera, RenderContext * context)const override;
			void Set(Func fun) { func = fun; }
			Func DepthFunc()const { return func; }
		private:
			Func func;
		};


		class GRAPHICS_EXPORT_IMPORT TextureSampler:public RenderStateIndexed

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


	}
}

#endif