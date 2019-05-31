
#include "vm_largevolumeraycast.h"

#include "rendercontext.h"

namespace ysl
{
	namespace vm
	{
		void VM_LargeVolumeRayCast::InitEvent()
		{

			SetAggregation(MakeRef<Aggregate>());


			Vec2i viewportSize{ 800,600 };

			GetAggregate()->GetCamera()->GetViewport()->SetViewportSize(viewportSize.x,viewportSize.y);


			auto fbo = Context()->CreateFramebufferObject(viewportSize.x,viewportSize.y, RDB_COLOR_ATTACHMENT0, RDB_COLOR_ATTACHMENT0);
			fbo->AddDepthStencilAttachment(MakeRef<FBODepthStencilAttachment>(DSBT_DEPTH24_STENCIL8));

			// Create a texture by given params
			auto texture = MakeRef<Texture>();
			auto texCreateParams = MakeRef<TexCreateParams>();
			texCreateParams->SetSize(viewportSize.x, viewportSize.y,0);
			texCreateParams->SetTextureFormat(TF_RGBA32F);
			texCreateParams->SetTextureTarget(TD_TEXTURE_RECTANGLE);
			texture->SetSetupParams(texCreateParams);
			assert(texture->CreateTexture());
			fbo->AddTextureAttachment(AP_COLOR_ATTACHMENT0, MakeRef<FBOTextureAttachment>(texture));
			GetAggregate()->Renderers()[0]->SetFramebuffer(fbo);

		}

		void VM_LargeVolumeRayCast::UpdateScene()
		{

		}

		void VM_LargeVolumeRayCast::SetupShading()
		{

		}
	}
}