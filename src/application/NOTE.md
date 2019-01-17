


	Application is far away from completion. A perfect Application should process events from system.
It's not its responsibility to determinate the graphic API. But the Application is play the both roles with GLFW
such that those features can be implemented easiliy.
	

		AbstractApplication                                                                   \
                ^                                                                             |
				|												                              |
  Platform-specific Application (e.g. Console, Windows, Linux, macOS...,                      |
                                 Process platform-specific events such                        |
                                as from network and window)                                   |
																							  |-> All GLFW do so far
                                                                                              |
        Renderer                                                                              |
           ^                                                                                  |
           |                                                                                  |
  Graphics API-specific Renderer (e.g. OpenGL, D3D, Vulkan, Metal even software rasterization |
                                  or Ray tracing)                                             /