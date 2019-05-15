
#ifndef _EVALUATOR_H_
#define _EVALUATOR_H_

namespace ysl
{
	namespace graphics
	{
		class Camera;
		class Actor;

		class LODEvaluator
		{
		public:
			LODEvaluator() = default;
			int Eval(const Actor * actor, const Camera * camera);
			virtual ~LODEvaluator() = 0;
		private:
		};
	}
}
#endif
