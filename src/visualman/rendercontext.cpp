
#include <GL/gl3w.h>
#include "rendercontext.h"
#include "vertexattribsetinterface.h"
#include "graphictype.h"
#include "abstraarray.h"
#include "renderstateset.h"
#include "enableset.h"
#include "abstrarenderstate.h"
#include "ogl.h"
#include "renderstate.h"
#include <cassert>
#include <vector>

namespace ysl
{
namespace vm
{
RenderContext::RenderContext() :
  framebuffer( MakeRef<Framebuffer>( this, 800, 600, RDB_BACK_LEFT, RDB_BACK_LEFT ) )  // A default frambuffer
{
	int width = 800, height = 600;
}

RenderContext::RenderContext( int w, int h ) :
  framebuffer( MakeRef<Framebuffer>( this, w, h, RDB_BACK_LEFT, RDB_BACK_LEFT ) )
{
}

bool RenderContext::InitContext()  // can be seemed as InitGLResources()
{
	MakeCurrent();
	gl3wInit();
	if ( !gl3wIsSupported( 4, 4 ) ) {
		throw std::runtime_error( "OpenGL 4.4 or higher is needed." );
	}

	/// TODO:: Lot of works to do here for robust.

	GetOpenGLExtensions();
	GetMaxInteger();
	InitDefaultRenderState();

#ifndef NDBUG
	glEnable( GL_DEBUG_OUTPUT );
	glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
	glDebugMessageCallback( (GLDEBUGPROC)gl_debug_msg_callback, NULL );
	glDebugMessageControlARB( GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE );
#endif
	initialized = true;
}

void RenderContext::DestroyGLResources()
{
	// Destroy all gl resources
	InitDefaultRenderEnable();
	InitDefaultRenderEnable();

	std::vector<Ref<IEventListener>>().swap( listeners );
	std::vector<Ref<FramebufferObject>>().swap( framebufferObjects );
	framebuffer = nullptr;
	curProgram = nullptr;
	std::unordered_map<RenderStateType, RenderStateBox>().swap( currentRenderStates );
	std::unordered_set<EnableState>().swap( currentEnableStates );
}

void RenderContext::SetContextFormat( const RenderContextFormat &fmt )
{
	format = fmt;
}

void RenderContext::Terminate()
{
	terminate = true;
}

bool RenderContext::IsTerminate()
{
	return terminate;
}

Ref<Framebuffer> RenderContext::GetFramebuffer()
{
	assert( framebuffer );
	return framebuffer;
}

Ref<FramebufferObject> RenderContext::CreateFramebufferObject()
{
	return CreateFramebufferObject( 0, 0, RDB_COLOR_ATTACHMENT0, RDB_COLOR_ATTACHMENT0 );
}

Ref<FramebufferObject> RenderContext::CreateFramebufferObject( int width, int height, ReadDrawBuffer readBuffer,
															   ReadDrawBuffer drawBuffer )
{
	framebufferObjects.push_back( MakeRef<FramebufferObject>( this, width, height, readBuffer, drawBuffer ) );
	framebufferObjects.back()->CreateFrambufferObject();
	return framebufferObjects.back();
}

void RenderContext::AddEventListener( Ref<IEventListener> listener )
{
	if ( listener && listener->context == nullptr ) {
		listener->context = this;
		if ( IsInitialized() )
			listener->InitEvent();
		listener->AddedEvent( this );
		listeners.push_back( std::move( listener ) );
	}
}

void RenderContext::RemoveEventListener( Ref<IEventListener> listener )
{
	if ( listener && listener->context == this ) {
		for ( auto it = listeners.begin(); it != listeners.end(); ) {
			if ( *it == listener ) {
				it = listeners.erase( it );
				listener->DeletedEvent( this );
			} else {
				++it;
			}
		}
	}
}

void RenderContext::DispatchInitEvent()
{
	//std::cout << " RenderContext::DispatchInitEvent\n";
	MakeCurrent();
	if ( IsInitialized() ) {
		for ( const auto &each : listeners ) {
			if ( each->Enabled() )
				each->InitEvent();
		}
	}
}

void RenderContext::DispatchUpdateEvent()
{
	//std::cout << "RenderContext::DispatchUpdateEvent\n";
	MakeCurrent();
	for ( const auto &each : listeners ) {
		if ( each->Enabled() )
			each->UpdateEvent();
	}
}

void RenderContext::DispatchDestroyEvent()
{
	//std::cout << "RenderContext::DispatchDestroyEvent\n";
	MakeCurrent();
	for ( const auto &each : listeners ) {
		if ( each->Enabled() )
			each->DestroyEvent();
	}
	DestroyGLResources();
}

void RenderContext::DispatchResizeEvent( int w, int h )
{
	//std::cout << "RenderContext::DispatchResizeEvent:" << w << " " << h << std::endl;
	MakeCurrent();
	// Update Frambuffer Size
	framebuffer->SetWidth( w );
	framebuffer->SetHeight( h );
	for ( const auto &each : listeners ) {
		if ( each->Enabled() )
			each->ResizeEvent( w, h );
	}
}

void RenderContext::DispatchMousePressedEvent( MouseButton button, int xpos, int ypos )
{
	//std::cout << "DispatchMousePressedEvent:" << button<<" "<<xpos << " " << ypos << std::endl;
	MakeCurrent();
	for ( const auto &each : listeners ) {
		if ( each->Enabled() )
			each->MousePressEvent( button, xpos, ypos );
	}
}

void RenderContext::DispatchMouseMoveEvent( MouseButton button, int xpos, int ypos )
{
	//std::cout << "RenderContext::DispatchMouseMoveEvent:" << button << " " << xpos << " " << ypos << std::endl;
	MakeCurrent();
	for ( const auto &each : listeners ) {
		if ( each->Enabled() )
			each->MouseMoveEvent( button, xpos, ypos );
	}
}

void RenderContext::DispatchMouseReleasedEvent( MouseButton button, int xpos, int ypos )
{
	//std::cout << "RenderContext::DispatchMouseReleasedEvent:" << button << " " << xpos << " " << ypos << std::endl;
	MakeCurrent();
	for ( const auto &each : listeners ) {
		if ( each->Enabled() )
			each->MouseReleaseEvent( button, xpos, ypos );
	}
}

void RenderContext::DispatchMouseWheelEvent( int ydegree, int xdegree )
{
	//std::cout << "RenderContext::DispatchMouseWheelEvent:" <<ydegree << std::endl;
	MakeCurrent();
	for ( const auto &each : listeners ) {
		if ( each->Enabled() )
			each->MouseWheelEvent( ydegree, xdegree );
	}
}

void RenderContext::DispatchKeyReleasedEvent( KeyButton key )
{
	//std::cout << "RenderContext::DispatchKeyReleasedEvent:" << key << std::endl;
	MakeCurrent();
	for ( const auto &each : listeners ) {
		if ( each->Enabled() )
			each->KeyReleaseEvent( key );
	}
}

void RenderContext::DispatchKeyPressedEvent( KeyButton key )
{
	//std::cout << "RenderContext::DispatchKeyPressedEvent:" << key << std::endl;
	MakeCurrent();
	for ( const auto &each : listeners ) {
		if ( each->Enabled() )
			each->KeyPressEvent( key );
	}
}

void RenderContext::DispatchFileDropEvent( const std::vector<std::string> &fileNames )
{
	MakeCurrent();
	for ( const auto &each : listeners ) {
		if ( each->Enabled() ) {
			each->FileDropEvent( fileNames );
		}
	}
}

void RenderContext::UseProgram( Ref<const GLSLProgram> program )
{
	assert( program );
	GL( glUseProgram( program->Handle() ) );
	curProgram = std::move( program );
}

void RenderContext::BindVertexArray( const IVertexAttribSet *vas )
{
	assert( false );
	//if (vas)
	//{
	//	for (auto i = 0; i < maxInteger.MAX_VERTEX_ATTRIBS; i++)
	//	{
	//		const auto ptr = vas->GetVertexAttribArray(i);
	//		if (ptr == nullptr)
	//		{
	//			// if there is no corresponding generic vertex array, disable it.
	//			if(vertexAttributeInfo[i].enabled)
	//			{
	//				vertexAttributeInfo[i].enabled = false;
	//				vertexAttributeInfo[i].vertexBufferHandle = 0;
	//				GL(glDisableVertexAttribArray(i));
	//			}

	//			GL(glDisableVertexAttribArray(i));

	//		}
	//		else
	//		{
	//			// If there is a coreesponding generic vertex array, enable it
	//			if(!vertexAttributeInfo[i].enabled)
	//			{
	//				vertexAttributeInfo[i].enabled = true;
	//				//vertexAttributeInfo[i].vertexBufferHandle = ptr->GetBufferObject()->Handle();
	//				GL(glEnableVertexAttribArray(i));
	//			}

	//			const auto curBufferObjectHandle = ptr->GetBufferObject()->Handle();
	//			if(curBufferObjectHandle != vertexAttributeInfo[i].vertexBufferHandle)
	//			{
	//				vertexAttributeInfo[i].vertexBufferHandle = curBufferObjectHandle;

	//				GL(glBindBuffer(GL_ARRAY_BUFFER,curBufferObjectHandle));

	//				GL(glVertexAttribPointer(i, // attribute location
	//					ptr->ComponentNum(), // component of vector
	//					ptr->Type(), // type enum
	//					GL_FALSE,	// normalized
	//					0,		// stride
	//					reinterpret_cast<void*>(0)));		// offset

	//				Debug("{} {}",ptr->ComponentNum(),ptr->Type());

	//			}
	//		}
	//	}
	//}
}

void RenderContext::Bind_VAO( int vbo_handle )
{
	GL( glBindVertexArray( vbo_handle ) );
}

void RenderContext::ApplyRenderState( const RenderStateSet *rss )
{
	if ( !rss ) {
		for ( const auto &each : defaultRenderStates ) {
			each.Apply( nullptr, this );
		}

		std::unordered_map<RenderStateType, RenderStateBox>().swap( currentRenderStates );

		return;
	}

	assert( rss );
	// Set the new render state and reset all states that
	// don't exist in the new render state list as default

	std::unordered_map<RenderStateType, RenderStateBox> newStates;

	for ( const auto &each : rss->renderStates ) {
		const auto type = each.StateType();  // Indexed or non-indexed
		auto it = currentRenderStates.find( type );
		newStates[ type ] = each;

		if ( it == currentRenderStates.end() || ( it->second.rawRenderState != each.rawRenderState ) ) {
			//If the state don't exist in current states or the value of new state is not the same as before
			each.Apply( nullptr, this );
		}
	}

	for ( const auto &each : currentRenderStates ) {
		const auto curStateType = each.first;

		auto it = newStates.find( curStateType );
		if ( it == newStates.end() ) {
			// If the state don't exist in new state
			defaultRenderStates[ curStateType ].Apply( nullptr, this );
		}
	}

	std::swap( currentRenderStates, newStates );
}

void RenderContext::ApplyRenderEnable( const EnableStateSet *ess )
{
	if ( !ess ) {
		for ( int i = 0; i < sizeof( EnableEnum2GLEnum ) / sizeof( GLenum ); i++ ) {
			GL( glDisable( EnableEnum2GLEnum[ i ] ) );
		}
		std::unordered_set<EnableState>().swap( currentEnableStates );
		return;
	}

	// Set the new enable state and reset all enable state that
	// don't exist in the new enable state list as disabled
	assert( ess );
	std::unordered_set<EnableState> newEnableStates;

	for ( const auto &each : ess->enableSet ) {
		auto it = currentEnableStates.find( each );
		newEnableStates.insert( each );
		if ( it == currentEnableStates.end() ) {
			assert( each >= 0 && each < sizeof( EnableEnum2GLEnum ) / sizeof( GLenum ) );
			GL( glEnable( EnableEnum2GLEnum[ each ] ) );
		}
	}

	for ( const auto &each : currentEnableStates ) {
		auto it = newEnableStates.find( each );
		if ( it == newEnableStates.end() ) {
			assert( each >= 0 && each < sizeof( EnableEnum2GLEnum ) / sizeof( GLenum ) );
			GL( glDisable( EnableEnum2GLEnum[ each ] ) );
		}
	}

	std::swap( currentEnableStates, newEnableStates );
}

void RenderContext::gl_debug_msg_callback( GLenum source, GLenum type, GLuint id, GLenum severity,
										   GLsizei length, const GLchar *message, void *userParam )
{
	(void)userParam;

	if ( id == 131169 || id == 131185 || id == 131218 || id == 131204 ) return;

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	switch ( source ) {
	case GL_DEBUG_SOURCE_API: std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY: std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION: std::cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER: std::cout << "Source: Other"; break;
	}
	std::cout << std::endl;

	switch ( type ) {
	case GL_DEBUG_TYPE_ERROR: std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: std::cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY: std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE: std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER: std::cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP: std::cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP: std::cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER: std::cout << "Type: Other"; break;
	}
	std::cout << std::endl;

	switch ( severity ) {
	case GL_DEBUG_SEVERITY_HIGH: std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM: std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW: std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	}
	std::cout << std::endl;
	std::cout << std::endl;
}

void RenderContext::GetMaxInteger()
{
	// Get max integer at run-time
	GL( glGetIntegerv( GL_MAX_VERTEX_ATTRIBS, &maxInteger.MAX_VERTEX_ATTRIBS ) );
	maxInteger.MAX_VERTEX_ATTRIBS = std::min( int( VA_VertexAttribArray_Count ), int( maxInteger.MAX_VERTEX_ATTRIBS ) );
	GL( glGetIntegerv( GL_MAX_TEXTURE_IMAGE_UNITS, &maxInteger.MAX_TEXTURE_IMAGE_UNITE ) );
	maxInteger.MAX_TEXTURE_IMAGE_UNITE = std::min( int( RS_TextureSampler15 - RS_TextureSampler0 + 1 ), int( maxInteger.MAX_TEXTURE_IMAGE_UNITE ) );
	GL( glGetIntegerv( GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &maxInteger.MAX_SHADER_STORAGE_BINDINGS ) );
	maxInteger.MAX_SHADER_STORAGE_BINDINGS = std::min( int( RS_ShaderStorageBuffer7 - RS_ShaderStorageBuffer0 + 1 ), maxInteger.MAX_SHADER_STORAGE_BINDINGS );
	GL( glGetIntegerv( GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, &maxInteger.MAX_ATOMIC_COUNTER_BUFFER_BINDINGS ) );
	maxInteger.MAX_ATOMIC_COUNTER_BUFFER_BINDINGS = std::min( int( RS_AtomicCounterBuffer7 - RS_AtomicCounterBuffer0 + 1 ), maxInteger.MAX_ATOMIC_COUNTER_BUFFER_BINDINGS );
	GL( glGetIntegerv( GL_MAX_IMAGE_UNITS, &maxInteger.MAX_IMAGE_UNITS ) );
	maxInteger.MAX_IMAGE_UNITS = std::min( int( RS_TextureImageUnit15 - RS_TextureSampler + 1 ), maxInteger.MAX_IMAGE_UNITS );
	GL( glGetIntegerv( GL_MAX_3D_TEXTURE_SIZE, &maxInteger.MAX_3DTEXUTRE_SIZE ) );

	GL( glGetIntegerv( GL_MAX_COMBINED_UNIFORM_BLOCKS, &maxInteger.MAX_UNIFORM_BLOCKS_COUNT ) );
	maxInteger.MAX_UNIFORM_BLOCKS_COUNT = std::min( int( RS_UniformBuffer7 - RS_UniformBuffer0 + 1 ), maxInteger.MAX_UNIFORM_BLOCKS_COUNT );

	if ( CheckSupportForExtension( "GL_NVX_gpu_memory_info" ) ) {
		constexpr int GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX = 0x9047;		   // dedicated video memory, total size (in kb) of the GPU memory
		constexpr int GPU_MEMORY_INFO_TOTAL_AVAILABEL_MEMORY_NVX = 0x9048;	 // total available memory, total size (in Kb) of the memory available for allocations
		constexpr int GPU_MEMORY_INFO_CURRENT_AVAILABEL_VIDEMEM_NVX = 0x9049;  //current available dedicated video memory (in kb), currently unused GPU memory
		GL( glGetIntegerv( GPU_MEMORY_INFO_TOTAL_AVAILABEL_MEMORY_NVX, &maxInteger.MAX_GPU_MEMORY_SIZE ) );

	} else if ( CheckSupportForExtension( "GL_ATI_meminfo" ) ) {
		constexpr int GPU_VBO_FREE_MEMORY_ATI = 0x87FB;
		constexpr int GPU_TEXTURE_FREE_MEMORY_ATI = 0x87FC;
		constexpr int GPU_RENDERBUFFER_FREE_MEMORY_ATI = 0x87FD;
		int texFreeMem = 0, vboFreeMem = 0, renderBufferFreeMem = 0;
		GL( glGetIntegerv( GPU_TEXTURE_FREE_MEMORY_ATI, &texFreeMem ) );
		GL( glGetIntegerv( GPU_VBO_FREE_MEMORY_ATI, &vboFreeMem ) );
		GL( glGetIntegerv( GPU_RENDERBUFFER_FREE_MEMORY_ATI, &renderBufferFreeMem ) );
		maxInteger.MAX_GPU_MEMORY_SIZE = texFreeMem + vboFreeMem + renderBufferFreeMem;
	}

	::vm::Log( "MAX_VERTEX_ATTRIBS:{}\n", maxInteger.MAX_VERTEX_ATTRIBS );
	::vm::Log( "MAX_TEXTURE_IMAGE_UNITE:{}\n", maxInteger.MAX_TEXTURE_IMAGE_UNITE );
	::vm::Log( "MAX_SHADER_STORAGE_BINDINGS:{}\n", maxInteger.MAX_SHADER_STORAGE_BINDINGS );
	::vm::Log( "MAX_ATOMIC_COUNTER_BUFFER_BINDINGS:{}\n", maxInteger.MAX_ATOMIC_COUNTER_BUFFER_BINDINGS );
	::vm::Log( "MAX_IMAGE_UNITS:{}\n", maxInteger.MAX_IMAGE_UNITS );
	::vm::Log( "MAX_3DTEXTURE_SIZE:{}\n", maxInteger.MAX_3DTEXUTRE_SIZE );
	::vm::Log( "MAX_UNIFORM_BLOCKS_COUNT:{}\n", maxInteger.MAX_UNIFORM_BLOCKS_COUNT );
	::vm::Log( "MAX_GPU_MEMORY_SIZE:{}", maxInteger.MAX_GPU_MEMORY_SIZE );
}

void RenderContext::InitDefaultRenderState()
{
	// Init some base render states
	//
	// Non-index state
	//defaultRenderStates[RS_AlphaFunc] = RenderStateBox(MakeRef<DepthFuncState>(FU_LESS), 0);

	defaultRenderStates[ RS_BlendFunc ] = RenderStateBox( MakeRef<BlendFuncState>( BF_SRC_COLOR, BF_SRC_ALPHA, BF_ONE_MINUS_SRC_COLOR, BF_ONE_MINUS_DST_ALPHA ), 0 );
	defaultRenderStates[ RS_CullFace ] = RenderStateBox( MakeRef<CullFaceState>( PF_BACK ), 0 );
	defaultRenderStates[ RS_DepthFunc ] = RenderStateBox( MakeRef<DepthFuncState>( FU_LESS ), 0 );
	defaultRenderStates[ RS_BlendEquation ] = RenderStateBox( MakeRef<BlendEquationState>( BE_FUNC_ADD, BE_FUNC_ADD ), 0 );
	defaultRenderStates[ RS_PolygonMode ] = RenderStateBox( MakeRef<PolygonModeState>( PM_FILL, PM_FILL ), 0 );
	defaultRenderStates[ RS_FrontFace ] = RenderStateBox( MakeRef<FrontFaceState>(), 0 );
	defaultRenderStates[ RS_LineWidth ] = RenderStateBox( MakeRef<LineWidthState>( 1.0 ), 0 );

	//defaultRenderStates[RS_StencilFunc] = RenderStateBox(new StencilFunc, 0);
	//defaultRenderStates[RS_StencilMask] = RenderStateBox(new StencilMask, 0);
	//defaultRenderStates[RS_StencilOp] = RenderStateBox(new StencilOp, 0);

	defaultRenderStates[ RS_GLSLProgram ] = RenderStateBox( MakeRef<GLSLProgram>(), 0 );

	// indexed state

	// Texture Sampler
	for ( int i = 0; i < maxInteger.MAX_TEXTURE_IMAGE_UNITE; i++ ) {
		defaultRenderStates[ RS_TextureSampler + i ] = RenderStateBox( MakeRef<TextureSampler>(), i );
	}

	// SSBO
	for ( int i = 0; i < maxInteger.MAX_SHADER_STORAGE_BINDINGS; i++ ) {
		defaultRenderStates[ RS_ShaderStorageBuffer + i ] = RenderStateBox( MakeRef<ShaderStorageBufferObject>(), i );
	}

	// AtomicCounterBuffer
	for ( int i = 0; i < maxInteger.MAX_ATOMIC_COUNTER_BUFFER_BINDINGS; i++ ) {
		defaultRenderStates[ RS_AtomicCounterBuffer + i ] = RenderStateBox( MakeRef<AtomicCounter>(), i );
	}

	// Image Units

	for ( int i = 0; i < maxInteger.MAX_IMAGE_UNITS; i++ ) {
		defaultRenderStates[ RS_TextureImageUnit + i ] = RenderStateBox( MakeRef<TextureImageUnit>(), i );
	}

	for ( int i = 0; i < maxInteger.MAX_UNIFORM_BLOCKS_COUNT; i++ ) {
		defaultRenderStates[ RS_UniformBuffer + i ] = RenderStateBox( MakeRef<UniformBufferObject>(), i );
	}
}

void RenderContext::InitDefaultRenderEnable()
{
}

void RenderContext::GetOpenGLExtensions()
{
	auto count = 0;
	GL( glGetIntegerv( GL_NUM_EXTENSIONS, &count ) );
	for ( int i = 0; i < count; ++i )
		extensions.emplace_back( (char *)( glGetStringi( GL_EXTENSIONS, i ) ) );
	GL_ERROR_REPORT;
}

bool RenderContext::CheckSupportForExtension( const std::string &ext )
{
	for ( const auto &e : extensions )
		if ( ext == e )
			return true;
	return false;
}

}  // namespace vm
}  // namespace ysl
