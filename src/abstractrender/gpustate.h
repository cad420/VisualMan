/*
   The MIT License

   Copyright (c) 2019


   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.
*/
#ifndef _STATE_H_
#define _STATE_H_

namespace ysl
{

	enum StateCull
	{
		CFRONT,
		CBACK
	};

	enum StateDepthFunc
	{
		DFNEVER,
		DFLESS,
		DFEQUAL,
		DFLEQUAL,
		DFGREATER,
		DFNOTEQUAL,
		DFGEQUAL,
		DFALWAYS
	};

	enum StateBlendEquation
	{
		
	};

	enum StateBlendFunc
	{
		
	};






	class GPUState
	{
	public:
		GPUState();
		bool enableDepthTest;
		DEPTH_FUNC depthFunc;
		bool enableCullFace;
		STATE_CULL cullState;
		bool enableBlend;
		bool enableScissor;
		bool enableLighting;
		bool enableLight[StateLightCount];
		bool enableColorMaterial;
		STATE_TEX enableTex[StateTUCount];
		size_t activeTexUnit;
		bool depthMask;
		bool colorMask;
		BLEND_EQUATION blendEquation;
		BLEND_FUNC blendFuncSrc;
		BLEND_FUNC blendFuncDst;
		float lineWidth;

	};

	class StateManager
	{
		
	};

}

#endif
