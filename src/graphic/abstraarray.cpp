
#include "abstraarray.h"

namespace ysl
{
	namespace vm
	{
		AbstraArray::AbstraArray()
		{
			bufferObject = MakeRef<BufferObject>();
		}

		void AbstraArray::UpdateBufferObject(BufferObjectUpdateMode mode)
		{
			bufferObject->SetBufferData(usage, mode & VM_UM_DiscardRAM);
			SetbufferObjectDataDirty(false);
		}
	}
}