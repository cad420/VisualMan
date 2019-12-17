
#ifndef _EVALUATOR_H_
#define _EVALUATOR_H_

namespace vm
{
class Camera;
class Actor;

class LODEvaluator
{
public:
	LODEvaluator() = default;
	int Eval( const Actor *actor, const Camera *camera );
	virtual ~LODEvaluator() = 0;

private:
};

inline int LODEvaluator::Eval( const Actor *actor, const Camera *camera )
{
	return 0;
}
}  // namespace vm
#endif
