
#include "art.h"
namespace vm
{
int Artist::EvalLOD( const Actor *actor, const Camera *camera )
{
	if ( lodEvaluator ) {
		return activeLOD = lodEvaluator->Eval( actor, camera );
	}
	return 0;
}

VMRef<Shading> Artist::GetShader( int lod, int pass )
{
	const auto ptr = GetLOD( lod );
	if ( ptr ) {
		pass = Clamp( pass, 0, ptr->size() );
		return ( *ptr )[ pass ];
	}
	return nullptr;
}

VMRef<ShadingPasses> Artist::GetLOD( int lod )
{
	lod = Clamp( lod, 0, MaxLodLevel );
	return LodShadingPasses[ lod ];
}
}  // namespace vm