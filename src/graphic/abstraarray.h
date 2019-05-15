
#ifndef _ABSTRAARRAY_H_
#define _ABSTRAARRAY_H_

#include "graphictype.h"
#include "bufferobject.h"
#include "../mathematics/transformation.h"

namespace ysl
{
	namespace graphics
	{
		class GRAPHICS_EXPORT_IMPORT AbstraArray
		{
		public:
			AbstraArray()=default;

			AbstraArray(const AbstraArray& other) = default;


			AbstraArray& operator=(const AbstraArray& other) = default;


			AbstraArray(AbstraArray&& other) noexcept = default;


			AbstraArray& operator=(AbstraArray&& other) noexcept = default;


			Ref<BufferObject> GetBufferObject() { return bufferObject; }
			char * Data() { return bufferObject ? bufferObject->Data() : nullptr; }
			const char * Data()const { return bufferObject?bufferObject->Data():nullptr; }
			void Clear() { if (bufferObject)bufferObject->Clear(); }
			std::size_t Size()const { return bufferObject ? bufferObject->Size() : 0; }


			virtual Bound3f GetBoundingBox()const = 0;
			virtual void SetTransform(const Transform & transform) = 0;
			virtual ~AbstraArray() = default;
		private:
			Ref<BufferObject> bufferObject;
		};
		template<typename Vec_Ty,typename Sca_Ty,size_t ScalarNum,unsigned TypeFlag>
		class Array:public AbstraArray
		{
		public:
			using ScalarType = Sca_Ty;
			using VectorType = Vec_Ty;
			virtual size_t ScalarNum()const { return ScalarNum(); }
			virtual unsigned Type()const { return TypeFlag; }
			virtual size_t SizeOfVectorType()const { return sizeof(VectorType); }
			size_t VectorCount()const { return Size() / SizeOfVectorType(); }
			const VectorType * Data()const { return reinterpret_cast<const VectorType*>(Data()); }
			VectorType * Data() { return reinterpret_cast<VectorType*>(Data()); }
			VectorType & operator[](size_t i) { return *(Data() + i); }
			const VectorType & operator[](size_t i)const { return *(Data() + i); }
			size_t Size()const {};
			virtual ~Array() = default;
		};
		class ArrayFloat1:public Array<float,float,1,GL_FLOAT> {};
		class ArrayFloat3:public Array<Vec3f, float, 1, GL_FLOAT> {};
	}
}

#endif