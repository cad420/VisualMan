
#ifndef _SHADING_H_
#define _SHADING_H_
#include "abstrarenderstate.h"
#include "uniform.h"
#include "enableset.h"
#include "renderstate.h"

#include "renderstateset.h"
#include <cassert>

namespace vm
{
class VISUALMAN_EXPORT_IMPORT Shading
{
public:
	Shading() = default;
	virtual ~Shading() = default;
	// Shading Program getter and setter
	VMRef<GLSLProgram> CreateGetProgram() { return CreateGetRenderStateSet()->CreateGetProgram(); }
	VMRef<GLSLProgram> GetProgram()
	{
		assert( GetRenderStateSet() );
		return GetRenderStateSet()->GetProgram();
	}
	VMRef<const GLSLProgram> GetProgram() const
	{
		assert( GetRenderStateSet() );
		return GetRenderStateSet()->GetProgram();
	}
	void SetProgram( VMRef<GLSLProgram> program )
	{
		assert( GetRenderStateSet() );
		GetRenderStateSet()->SetProgram( program );
	}

	// RenderState getter and setter
	VMRef<RenderStateSet> CreateGetRenderStateSet() { return renderStateSet ? renderStateSet : ( renderStateSet = MakeVMRef<RenderStateSet>() ); }
	VMRef<RenderStateSet> GetRenderStateSet() const { return renderStateSet; }
	void SetRenderState( VMRef<RenderState> state ) { CreateGetRenderStateSet()->SetRenderState( state, -1 ); }
	VMRef<DepthFuncState> GetDepthFuncState( RenderStateType type ) { return std::static_pointer_cast<DepthFuncState>( CreateGetRenderStateSet()->GetRenderState( type, -1 ) ); }

	VMRef<TextureSampler> CreateGetTextureSampler( int unitIndex );
	VMRef<TextureImageUnit> CreateGetTextureImageUnit( int unitIndex );
	VMRef<AtomicCounter> CreateGetAtomicCounter( int binding );
	VMRef<ShaderStorageBufferObject> CreateGetSSBO( int binding );
	/**
			 * \brief The binding point must be specified in GLSL
			 */
	VMRef<UniformBufferObject> CreateGetUBO( int binding );

	// EnableState getter and setter
	VMRef<EnableStateSet> CreateGetEnableStateSet() { return enableSet ? enableSet : ( enableSet = MakeVMRef<EnableStateSet>() ); }
	VMRef<EnableStateSet> GetEnableStateSet() { return enableSet; }
	VMRef<const EnableStateSet> GetEnableStateSet() const { return enableSet; }

	void SetDisableState( EnableState enable ) { CreateGetEnableStateSet()->Enable( enable ); }
	void SetEnableState( EnableState enable ) { CreateGetEnableStateSet()->Disable( enable ); }

	VMRef<UniformSet> CreateGetUniformSet() { return uniformSet ? uniformSet : ( uniformSet = MakeVMRef<UniformSet>() ); }
	VMRef<UniformSet> GetUniformSet() { return uniformSet; }
	VMRef<const UniformSet> GetUniformSet() const { return uniformSet; }

	// Uniform getter and setter
	void SetUniform( VMRef<Uniform> uniform ) { uniformSet->SetUniform( std::move( uniform ) ); }
	VMRef<Uniform> GetUniform( const char *name ) { return uniformSet->GetUniform( name ); }
	VMRef<Uniform> CreateGetUniform( const char *name ) { return CreateGetUniformSet()->CreateGetUniform( name ); }
	// texture unit
private:
	VMRef<RenderStateSet> renderStateSet;	 // shading program is in it
	VMRef<UniformSet> uniformSet;
	VMRef<EnableStateSet> enableSet;
};

VMRef<Shading> VISUALMAN_EXPORT_IMPORT MakePhongShading();
}  // namespace vm

#endif
