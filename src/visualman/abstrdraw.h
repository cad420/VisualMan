
#ifndef _ABSTRADRAW_H_
#define _ABSTRADRAW_H_

#include "graphictype.h"

namespace ysl
{
	namespace vm
	{


		//class  GRAPHICS_EXPORT_IMPORT IDrawCallEvent 
		//{
		//public:
		//	virtual void OnDrawCallFinishedEvent(
		//		const AbstraDrawCall* drawcall,
		//		const Actor* actor,
		//		const Shading* shading,
		//		const Camera* camera,
		//		RenderContext* context) = 0;
		//};

		class VISUALMAN_EXPORT_IMPORT AbstrDrawCall
		{
		public:
			AbstrDrawCall():drawType(PT_TRIANGLES){}

			virtual void Render()const = 0;

			virtual void UpdateDirtyBufferObject(BufferObjectUpdateMode mode) = 0;
			virtual void DestroyBufferObject() = 0;

			void SetPrimitiveType(PrimitiveType type) { drawType = type; }
			PrimitiveType GetPrimitiveType()const { return drawType; }
			virtual ~AbstrDrawCall() = default;
		private:
			PrimitiveType drawType;
		};
	}
}
#endif