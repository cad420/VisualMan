
#ifndef _RENDERCONTEXT_H_
#define _RENDERCONTEXT_H_

#include <memory>
#include <vector>
#include <array>

#include <VMat/geometry.h>
#include "graphictype.h"
#include "framebuffer.h"
#include "eventinterface.h"
#include "framebufferobject.h"
#include "shaderprogram.h"
#include <unordered_map>
#include <unordered_set>

namespace vm
{
class EnableStateSet;
class IVertexAttribSet;
class RenderStateSet;
class RenderStateBox;

enum class ContextProfile
{
	Core,
	Compatibility,
	Default
};

class RenderContextFormat
{
public:
	RenderContextFormat() :
	  rGBABits( 8, 8, 8, 0 ),
	  accumRGBABits( 0, 0, 0, 0 ),
	  nDepthBufferBits( 24 ),
	  stencilBufferBits( 8 ),
	  nMultiSamples( 16 )
	{
	}

	void SetRGBABits( const Vec4i &bits ) { rGBABits = bits; }
	void SetAccumRGBABits( const Vec4i &bits ) { accumRGBABits = bits; }
	void SetDoubleBuffer( bool on ) { doubleBuffer = on; }
	void SetMultiSample( bool on ) { bMultiSample = on; }
	void SetMultiSampleNumber( int num ) { nMultiSamples = num; }
	void SetDepthBufferBits( int num ) { nDepthBufferBits = num; }
	void SetStencilBufferBits( int num ) { stencilBufferBits = num; }

	Vec4i GetRGBABits() const { return rGBABits; }
	Vec4i GetAccumRGBABits() const { return accumRGBABits; }
	bool HasDoubleBuffer() const { return doubleBuffer; }
	bool MultiSample() const { return bMultiSample; }
	int GetMultiSampleNumber() const { return nMultiSamples; }
	int GetDepthBufferBits() const { return nDepthBufferBits; }
	int GetStencilBufferBits() const { return stencilBufferBits; }

	ContextProfile Profile() const { return ContextProfile::Core; }

private:
	Vec4i rGBABits;
	Vec4i accumRGBABits;
	int nDepthBufferBits;
	int stencilBufferBits;
	int nMultiSamples;
	int contextClientVersion;
	int majorVersion = 4;
	int minorVersion = 4;
	bool doubleBuffer = true;
	bool bMultiSample = false;
};

enum ContextState
{
	Context_OnRenderingStarted,
	Context_OnRenderingFinished
};

class VISUALMAN_EXPORT_IMPORT RenderContext
{
public:
	RenderContext();
	RenderContext( int w, int h );
	virtual ~RenderContext() = default;
	RenderContext( const RenderContext & ) = delete;
	RenderContext &operator=( const RenderContext & ) = delete;
	virtual void SwapBuffer() = 0;
	virtual void MakeCurrent() = 0;
	virtual void Update() = 0;

	bool InitContext();	 // We use gl3w
	bool GetAutomaticallyUpdate() const { return enableUpdate; }
	void SetAutomaticallyUpdate( bool update ) { enableUpdate = update; }
	void SetContextFormat( const RenderContextFormat &fmt );
	bool HasDoubleBuffer() const { return format.HasDoubleBuffer(); }
	bool IsInitialized() const { return initialized; }
	void SetContextState( ContextState state ) { contextState = state; }
	ContextState GetContextState() const { return contextState; }
	std::size_t GetDeviceTotalMemorySize() { return maxInteger.MAX_GPU_MEMORY_SIZE; }
	void Terminate();
	bool IsTerminate();

	virtual void SetWindowTitle( const std::string &title ) {}
	/**
			 * \brief  Returns default framebuffer
			 */
	VMRef<Framebuffer> GetFramebuffer();

	VMRef<FramebufferObject> CreateFramebufferObject();
	/**
			 * \brief  Creates and returns a FBO with given parameters
			 */
	VMRef<FramebufferObject> CreateFramebufferObject( int width, int height, ReadDrawBuffer readBuffer, ReadDrawBuffer drawBuffer );

	virtual void DestroyGLResources();
	void AddEventListener( VMRef<IEventListener> listener );
	void RemoveEventListener( VMRef<IEventListener> listener );
	// Event Call
	virtual void DispatchInitEvent();
	virtual void DispatchUpdateEvent();
	virtual void DispatchDestroyEvent();
	virtual void DispatchResizeEvent( int w, int h );
	virtual void DispatchMousePressedEvent( MouseButton button, int xpos, int ypos );
	virtual void DispatchMouseMoveEvent( MouseButton button, int xpos, int ypos );
	virtual void DispatchMouseReleasedEvent( MouseButton, int xpos, int ypos );
	virtual void DispatchMouseWheelEvent( int ydegree, int xdegree );
	virtual void DispatchKeyReleasedEvent( KeyButton key );
	virtual void DispatchKeyPressedEvent( KeyButton key );
	virtual void DispatchFileDropEvent( const std::vector<std::string> &fileNames );
	//Rendering State

	// glUseProgram
	void UseProgram( VMRef<const GLSLProgram> program );
	VMRef<const GLSLProgram> GetCurrentProgram() const { return curProgram; }

	// glVertexAttribArray
	void BindVertexArray( const IVertexAttribSet *vas );
	void Bind_VAO( int vbo_handle );
	void ApplyRenderState( const RenderStateSet *rss );
	void ApplyRenderEnable( const EnableStateSet *ess );

private:
	static void gl_debug_msg_callback( GLenum source,
									   GLenum type,
									   GLuint id,
									   GLenum severity,
									   GLsizei length,
									   const GLchar *message,
									   void *userParam );

	void GetMaxInteger();

	void InitDefaultRenderState();
	void InitDefaultRenderEnable();

	void GetOpenGLExtensions();
	bool CheckSupportForExtension( const std::string &ext );

	struct MAXINTEGER
	{
		int MAX_VERTEX_ATTRIBS = 0;
		int MAX_TEXTURE_IMAGE_UNITE = 0;
		int MAX_SHADER_STORAGE_BINDINGS = 0;
		int MAX_ATOMIC_COUNTER_BUFFER_BINDINGS = 0;
		int MAX_IMAGE_UNITS = 0;
		int MAX_3DTEXUTRE_SIZE = 0;
		int MAX_UNIFORM_BLOCKS_COUNT = 0;
		int MAX_GPU_MEMORY_SIZE = 0;
	} maxInteger;

	// extension string
	std::vector<std::string> extensions;

	//GLFWwindow * windowContext;
	std::vector<VMRef<IEventListener>> listeners;
	std::vector<VMRef<FramebufferObject>> framebufferObjects;
	VMRef<Framebuffer> framebuffer;

	RenderContextFormat format;
	bool enableUpdate = true;
	bool initialized = false;
	bool terminate = false;
	ContextState contextState = Context_OnRenderingFinished;

	// Render State

	// Current GLSLProgram

	VMRef<const GLSLProgram> curProgram = nullptr;
	std::array<RenderStateBox, RS_RenderState_Count> defaultRenderStates;
	std::array<EnableState, EN_EnableState_Count> defaultEnables;
	std::unordered_map<RenderStateType, RenderStateBox> currentRenderStates;
	std::unordered_set<EnableState> currentEnableStates;
};

}  // namespace vm

#endif
