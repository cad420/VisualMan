
#include "abstraarray.h"

namespace vm
{
AbstraArray::AbstraArray()
{
	bufferObject = MakeVMRef<BufferObject>();
}

void AbstraArray::UpdateBufferObject( BufferObjectUpdateMode mode )
{
	bufferObject->ReallocBufferData( usage, mode & VM_UM_DiscardRAM );
	//std::cout << "update\n";
	SetbufferObjectDataDirty( false );
}
}  // namespace vm