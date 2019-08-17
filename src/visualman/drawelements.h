
#ifndef _DRAWELEMENTS_H_
#define _DRAWELEMENTS_H_

#include <GL/gl3w.h>

#include "graphictype.h"
#include "abstrdraw.h"
#include "abstraarray.h"


namespace ysl
{
	namespace vm
	{

		class DrawElementsBase :public AbstrDrawCall {
		public:
			//virtual bool PrimitiveRestartEnable()const { return enablePrimitiveRestart; }
			DrawElementsBase() = default;
			void SetInstanceCount(int count) { instanceCount = count; }
			int GetInstanceCount()const { return instanceCount; }
			void SetBaseVertex(int baseVertex) { this->baseVertex = baseVertex; }
			unsigned int GetBaseVertex()const { return this->baseVertex; }
			
		private:
			int instanceCount = 0;
			bool enablePrimitiveRestart = false;
			unsigned int baseVertex = 0;
		};


		template<typename ArrayType>
		class DrawElements:public DrawElementsBase
		{
		public:

			using IndexType = typename ArrayType::ScalarType;

			DrawElements(int instances = 1):indexArray(MakeRef<ArrayType>())
			{
			}

			void SetCount(int count) { this->count = count; }
			int GetCount()const { return count; }
			void SetOffset(unsigned int offset) { this->offset = offset; }
			unsigned int GetOffset()const { return this->offset; }
			void SetIndexBuffer(Ref<ArrayType> array) { indexArray = std::move(array); }
			Ref<ArrayType> GetIndexArray() { return indexArray; }
			Ref<const ArrayType> GetIndexArray()const { return indexArray; }
			void UpdateDirtyBufferObject(BufferObjectUpdateMode mode) override;
			void DestroyBufferObject() override;
			void Render() const override;
		private:
			Ref<ArrayType> indexArray;
			int count = -1;
			unsigned int offset=0;
		};

		template <typename ArrayType>
		void DrawElements<ArrayType>::UpdateDirtyBufferObject(BufferObjectUpdateMode mode)
		{
			if (indexArray && indexArray->GetBufferObject() && indexArray->IsBufferObjectDataDirty())
			{
				//indexArray->GetBufferObject()->SetBufferData(BU_STATIC_DRAW, mode & VM_UM_DiscardRAM);		// upload data to GPU
				//indexArray->SetBufferObjectDataDirty(false);
				indexArray->UpdateBufferObject(mode);
			}
		}

		template <typename ArrayType>
		void DrawElements<ArrayType>::DestroyBufferObject()
		{
			if (indexArray && indexArray->GetBufferObject())
				indexArray->GetBufferObject()->DestroyBufferObject();
		}

		template <typename ArrayType>
		void DrawElements<ArrayType>::Render() const
		{

			const auto bufferObject = indexArray->GetBufferObject();

			if (bufferObject == nullptr)
			{
				// We only support that vertex indices are stored in GPU/
				// If they are not in gpu, just return.
				ysl::Warning("There is no bufferobject in DrawElements call");
				return;
			}

			const void* beginPtr = bufferObject->LocalData();
			const void * endPtr = nullptr;
			const unsigned int handle = bufferObject->Handle();

			if(handle)		// use index array in gpu
			{
				GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle));
				beginPtr = 0;
			}
			else  // use index array in cpu
			{
				GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
			}


			if(count < 0)		// use all vertex indices
			{
				beginPtr = reinterpret_cast<const char*>(beginPtr) + offset;
				endPtr = reinterpret_cast<const char *>(beginPtr) + bufferObject->BufferObjectSize() / sizeof(IndexType);
			}else
			{
				beginPtr = reinterpret_cast<const char*>(beginPtr) + offset;
				endPtr = reinterpret_cast<const char *>(beginPtr) + sizeof(IndexType)*count;
			}
			const auto cnt = (GLsizei)(reinterpret_cast<IndexType>(endPtr) - reinterpret_cast<IndexType>(beginPtr));
			//std::cout << "cnt:" << cnt << std::endl;
			GL(glLineWidth(GetLineWidth()));
			GL(glDrawElements(GetPrimitiveType(),cnt,ArrayType::TypeFlag,beginPtr));
		}

		class VISUALMAN_EXPORT_IMPORT DrawElementsUInt:public DrawElements<ArrayUInt>
		{
		public:
			DrawElementsUInt(int instance = 1) :DrawElements<ysl::vm::ArrayUInt>(instance){}
		};

	}
}

#endif
