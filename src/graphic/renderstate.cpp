
#include "renderstate.h"
#include <iostream>
#include "../opengl/openglutils.h"
#include "../../lib/gl3w/GL/gl3w.h"

#include "texture.h"

namespace ysl
{
	namespace vm
	{
		void DepthFuncState::Apply(int index, const Camera * camera, RenderContext* context) const
		{
			(void)(index);
			(void)(camera);
			(void)(context);
			GL(glDepthFunc(func));
		}

		void TextureSampler::Apply(int index, const Camera * camera, RenderContext* context) const
		{
			assert(index >= 0);
			unsigned int handle = 0;
			if (texture)
			{
				handle = texture->Handle();
				assert(handle);		// if texture exists but its handle is 0, failed.
			}
			GL(glBindTextureUnit(index, handle))
		}


		void BlendFuncState::Apply(int index, const Camera* camera, RenderContext* context) const
		{
			(void)(index);
			(void)(camera);
			(void)(context);
			GL(glBlendFuncSeparate(rgbSrc, rgbDest, alphaSrc, alphaDest));
		}

		void BlendEquationState::Apply(int index, const Camera* camera, RenderContext* context) const
		{
			(void)(index);
			(void)(camera);
			(void)(context);
			GL(glBlendEquationSeparate(rgbEquation, alphaEquation));
		}

		void CullFaceState::Apply(int index, const Camera* camera, RenderContext* context) const
		{
			(void)(index);
			(void)(camera);
			(void)(context);
			GL(glCullFace(face));
		}

		//void StencilOpState::Apply(int index, const Camera* camera, RenderContext* context) const
		//{

		//}

		//void StencilFuncState::Apply(int index, const Camera* camera, RenderContext* context) const
		//{

		//}

		//void StencilMaskState::Apply(int index, const Camera* camera, RenderContext* context) const
		//{
		//}

		//void ProgramState::Apply(int index, const Camera* camera, RenderContext* context) const
		//{
		//}

		void LineWidthState::Apply(int index, const Camera* camera, RenderContext* context) const
		{
			(void)(index);
			(void)(camera);
			(void)(context);
			GL(glLineWidth(lineWidth));
		}

		void PolygonModeState::Apply(int index, const Camera* camera, RenderContext* context) const
		{
			(void)(index);
			(void)(camera);
			(void)(context);
			if (frontMode == backMode)
			{
				GL(glPolygonMode(GL_FRONT_AND_BACK, frontMode));
			}
			else
			{
				GL(glPolygonMode(GL_FRONT, frontMode));
				GL(glPolygonMode(GL_BACK, backMode));
			}
		}
	}
}
