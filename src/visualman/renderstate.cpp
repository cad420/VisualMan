
#include <GL/gl3w.h>
#include "renderstate.h"
#include "rendercontext.h"
#include "graphictype.h"
#include "texture.h"
#include <cassert>

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

		void AtomicCounter::Apply(int index, const Camera* camera, RenderContext* context) const
		{
			assert(context);
			assert(index >= 0 && index < RS_AtomicCounterBuffer7 - RS_AtomicCounterBuffer0);
			if (bufferObject)	// If there is no bufferobject, apply a empty bufferobject
			{
				const unsigned int handle = bufferObject->Handle();
				assert(handle);
				GL(glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, index, handle));
			}
			else
			{
				GL(glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, index, 0));
			}
			//  GL_INVALID_ENUM is generated if target is not GL_ATOMIC_COUNTER_BUFFER, GL_TRANSFORM_FEEDBACK_BUFFER, GL_UNIFORM_BUFFER or GL_SHADER_STORAGE_BUFFER.
			//	GL_INVALID_VALUE is generated if index is greater than or equal to the number of target - specific indexed binding points.
			//	GL_INVALID_VALUE is generated if buffer does not have an associated data store, or if the size of that store is zero.
		}

		void ShaderStorageBufferObject::Apply(int index, const Camera* camera, RenderContext* context) const
		{
			assert(context);
			assert(index >= 0 && index < RS_ShaderStorageBuffer7 - RS_ShaderStorageBuffer0);
			if (bufferObject)	// If there is no bufferobject, apply a empty bufferobject
			{
				const unsigned int handle = bufferObject->Handle();
				assert(handle);
				GL(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, handle));
			}
			else
			{
				GL(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, 0));
			}

		}

		void TextureSampler::Apply(int index, const Camera * camera, RenderContext* context) const
		{
			//assert(index >= 0);
			unsigned int handle = 0;
			if (texture)
			{
				handle = texture->Handle();
				assert(handle);		// if texture exists but its handle is 0, failed.
				GL(glBindTextureUnit(index, handle));
			}
			else
			{
				GL(glBindTextureUnit(index, 0));
			}

		}

		void TextureImageUnit::Apply(int index, const Camera* camera, RenderContext* context) const
		{
			//assert(index >= 0);
			if (texture)
			{
				const unsigned int handle = texture->Handle();
				assert(handle);		// if texture exists but its handle is 0, failed.
				GL(glBindImageTexture(index, handle, 0, GL_FALSE, 0, VM_IA_READ_WRITE, texture->GetTextureFormat()));
			}
			else
			{
				// restore default state
				GL(glBindImageTexture(index, 0, 0, GL_FALSE, 0, VM_IA_READ_ONLY, GL_R8));
			}
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

		void FrontFaceState::Apply(int index, const Camera* camera, RenderContext* context) const
		{
			GL(glFrontFace(face));
		}

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
