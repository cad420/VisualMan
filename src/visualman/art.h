
#ifndef _ART_H_
#define _ART_H_
#include <array>
#include "shading.h"
#include "lodevaluator.h"

namespace vm
{
class Actor;

using ShadingPasses = std::vector<VMRef<Shading>>;

class VISUALMAN_EXPORT_IMPORT Artist
{
public:
	Artist()
	{
		LodShadingPasses[ 0 ] = MakeVMRef<ShadingPasses>();  // Lod of 0 is default.
	}

	int EvalLOD( const Actor *actor, const Camera *camera );

	VMRef<Shading> GetShader( int lod, int pass = 0 );

	//VMRef<ShadingPasses> CreateGetLOD(int lod);

	VMRef<ShadingPasses> GetLOD( int lod );

	void SetLODEvaluator( VMRef<LODEvaluator> evaluator ) { lodEvaluator = std::move( evaluator ); }

	VMRef<LODEvaluator> GetLODEvaluator() { return lodEvaluator; }

	void SetActiveLOD( int alod ) { activeLOD = alod; }

	int GetActiveLOD() const { return activeLOD; }

	//VMRef<LODEvaluator> GetLODEvaluator() { return lodEvaluator; }
private:
	static constexpr int MaxLodLevel = 8;
	std::array<VMRef<ShadingPasses>, MaxLodLevel> LodShadingPasses;
	VMRef<LODEvaluator> lodEvaluator = nullptr;
	int activeLOD = 0;
};
}  // namespace vm

#endif
