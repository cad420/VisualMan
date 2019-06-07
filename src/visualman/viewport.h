
#ifndef _VIEWPORT_H_
#define _VIEWPORT_H_
#include <geometry.h>
#include "graphictype.h"

namespace ysl
{
	namespace vm
	{
		class Viewport
		{
		public:
			Viewport(int w,int h):width(w),height(h){}
			Viewport(int x,int y,int w,int h):width(w),height(h),xpos(x),ypos(y){}
			void Activate()const;
			void SetViewportSize(int w, int h) { width = w; height = h; }
			void SetClearColor(const Vec4f & color) { clearColor = color; }
			void SetPosition(int x, int y) { xpos = x; ypos = y; }

			void SetXPos(int x) { xpos = x; }
			void SetYPos(int y) { ypos = y; }
			int GetXPos()const { return xpos; }
			int GetYPos()const { return ypos; }

			void SetWidth(int w) { width = w; }
			void SetHeight(int h) { height = h; }
			int GetWidth()const { return width; }
			int GetHeight()const { return height; }

			void SetClearStencil(int stencil) { clearStencil = stencil; }
			int GetClearStencil()const { return clearStencil; }

			void SetClearDepth(float depth) { clearDepth = depth; }
			float GetClearDept()const { return clearDepth; }

			void SetClearFlag(ClearFlag what) { clearWhat = what; }
			ClearFlag GetClearFlag()const { return clearWhat; }

		private:
			int width = 0;
			int height = 0;
			int xpos = 0;
			int ypos = 0;
			Vec4f clearColor = {1.f,1.f,1.f,1.f};
			ClearFlag clearWhat = CF_DO_NOT_CLEAR;
			float clearDepth = 1.0f;
			int clearStencil = 0;
		};

	}
}
#endif
