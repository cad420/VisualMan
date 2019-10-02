
#include "abstraarray.h"

namespace ysl
{
namespace vm
{
AbstraArray::AbstraArray()
{
	bufferObject = MakeRef<BufferObject>();
}

void AbstraArray::UpdateBufferObject( BufferObjectUpdateMode mode )
{
	bufferObject->ReallocBufferData( usage, mode & VM_UM_DiscardRAM );
	//std::cout << "update\n";
	SetbufferObjectDataDirty( false );
}
}  // namespace vm
}  // namespace ysl