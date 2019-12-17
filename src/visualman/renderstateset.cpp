#include "renderstateset.h"
#include "shaderprogram.h"

namespace vm
{
void RenderStateSet::SetRenderState( VMRef<RenderState> state, int index )
{
	if ( state == nullptr )
		return;

	if ( state->Type() == RS_GLSLProgram ) {
		program = std::static_pointer_cast<GLSLProgram>( state );
	}

	for ( auto &each : renderStates ) {
		if ( each.StateType() == state->Type() && each.index == index ) {
			each.rawRenderState = std::move( state );
			return;
		}
	}

	renderStates.push_back( RenderStateBox( std::move( state ), index ) );
}

void RenderStateSet::RemoveRenderState( RenderStateType type, int index )
{
	if ( type == RS_GLSLProgram ) {
		program = nullptr;
	}

	for ( auto it = renderStates.begin(); it != renderStates.end(); ) {
		if ( it->StateType() == type && it->index == index ) {
			it = renderStates.erase( it );
			if ( index != -1 )	// -1 indicates remove all
				return;
		} else
			++it;
	}
}

void RenderStateSet::SetProgram( VMRef<GLSLProgram> program )
{
	SetRenderState( program, -1 );
}

VMRef<GLSLProgram> RenderStateSet::CreateGetProgram()
{
	if ( program )
		return program;
	SetRenderState( MakeVMRef<GLSLProgram>(), -1 );
	return program;
}

VMRef<GLSLProgram> RenderStateSet::GetProgram()
{
	return program;
}

VMRef<RenderState> RenderStateSet::GetRenderState( RenderStateType type, int index )
{
	if ( type == RenderStateType::RS_GLSLProgram )
		return program;
	for ( auto &each : renderStates ) {
		if ( each.StateType() == type && each.index == index )
			return each.rawRenderState;
	}
	return nullptr;
}

}  // namespace vm