
#include "lvdfilereader.h"

namespace ysl
{
	IMPLEMENT_RTTI_NoConstructor(LVDFileReader,I3DBlockFilePluginInterface)
	//IMPLEMENT_INITIAL(LVDFileReader,common.pagefilereader.lvd)

}
EXPORT_PLUGIN_FACTORY_IMPLEMENT(ysl::LVDFileReaderFactory)
