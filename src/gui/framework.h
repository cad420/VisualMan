
#ifndef _FRAMEWORK_H_
#define _FRAMEWORK_H_


class CommandFramework
{
public:
	virtual void CommandCallback(const char * cmd) = 0;
	virtual void paintGL() = 0;
};

#endif
