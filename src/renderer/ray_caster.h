

#ifndef RAY_CASTER_H_
#define RAY_CASTER_H_

#include <gl/GL.h>
#include <GLFW/glfw3.h>

#include "../gui/framework.h"

class RayCasterDemo:public CommandFramework
{
public:
	RayCasterDemo();
	void CommandCallback(const char* cmd) override;
	void paintGL() override;
	 
};

#endif