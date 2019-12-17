
#ifndef _RENDERSTATESET_H_
#define _RENDERSTATESET_H_
#include "graphictype.h"
#include "abstrarenderstate.h"

namespace vm
{
class GLSLProgram;

class VISUALMAN_EXPORT_IMPORT RenderStateSet
{
	friend class Aggregate;
	friend class RenderContext;

public:
	RenderStateSet() = default;
	void SetRenderState( VMRef<RenderState> state, int index );
	VMRef<RenderState> GetRenderState( RenderStateType type, int index );
	void RemoveRenderState( RenderStateType type, int index );
	void SetProgram( VMRef<GLSLProgram> program );
	VMRef<GLSLProgram> CreateGetProgram();
	VMRef<GLSLProgram> GetProgram();
	//const auto & RenderStateBox()const { return renderStates; }
	//std::vector<RenderStateBox> RenderStateBox() { return renderStates; }
private:
	std::vector<RenderStateBox> renderStates;
	VMRef<GLSLProgram> program;
};

}  // namespace vm
#endif