
#ifndef _ART_H_
#define _ART_H_
#include <array>
#include "shading.h"
#include "lodevaluator.h"

namespace ysl
{
	namespace graphics
	{
		class Actor;

		using ShadingPasses = std::vector<Ref<Shading>>;
		
		class Artist
		{
		public:
			Artist()
			{
				LodShadingPasses[0] = MakeRef<ShadingPasses>();  // Lod of 0 is default.
			}

			int EvalLOD(const Actor* actor, const Camera* camera);

			Ref<Shading> GetShader(int lod, int pass = 0);
			
			//Ref<ShadingPasses> CreateGetLOD(int lod);

			Ref<ShadingPasses> GetLOD(int lod);

			void SetLODEvaluator(Ref<LODEvaluator> evaluator) { lodEvaluator = std::move(evaluator); }

			Ref<LODEvaluator> GetLODEvaluator() { return lodEvaluator; }

			void SetActiveLOD(int alod) { activeLOD = alod; }

			int GetActiveLOD()const { return activeLOD; }
			
			//Ref<LODEvaluator> GetLODEvaluator() { return lodEvaluator; }
		private:
			static constexpr int MaxLodLevel = 8;
			std::array<Ref<ShadingPasses>, MaxLodLevel> LodShadingPasses;
			Ref<LODEvaluator> lodEvaluator = nullptr;
			int activeLOD = 0;
		};
	}
}

#endif
