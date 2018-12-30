
#ifndef  _EVENTHANDLER_H_
#define _EVENTHANDLER_H_
#include "../mathematics/geometry.h"


class Event
{

};

class MouseEvent :public Event
{
public:
	enum { LeftButton = 1, RightButton = 2 };

	ysl::Point2i m_pos;
	int m_buttons;
	MouseEvent(const ysl::Point2i & pos, int button) :m_pos(pos), m_buttons(button) {}
	ysl::Point2i pos()const { return m_pos; }
	int buttons()const { return m_buttons; }
};

class KeyboardEvent :public Event
{
public:
	int m_key;
	KeyboardEvent(int key) :m_key(key) {}
	int key()const { return m_key; }
};

class ResizeEvent :public Event
{
public:
	ysl::Vector2i m_size;
	ResizeEvent(const ysl::Vector2i & size) :m_size(size) {}
	const ysl::Vector2i & size()const { return m_size; }
};

#endif