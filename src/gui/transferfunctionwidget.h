
#ifndef _TRANSFERFUNCTION_H_
#define _TRANSFERFUNCTION_H_

#include "imguibase.h"
#include <string>
#include "../opengl/texture.h"
#include "../../imgui/imgui.h"
//#include "../io/volume_utils.h"
#include "../utility/tfgenerator.h"

namespace ysl
{
	namespace imgui
	{
		class TransferFunctionWidget:
		                            public ImGuiInterface,
		                            public TransferFunction
		{
		public:
			TransferFunctionWidget() = default;
			TransferFunctionWidget(const std::string & fileName);
			void BindTexture(const std::shared_ptr<OpenGLTexture> & tex);
			void Draw() override;

		//protected:
		//	void MouseMoveEvent(MouseEvent * event) override;
		//	void MousePressEvent(MouseEvent * event) override;
		//	void MouseReleaseEvent(MouseEvent *event) override;
		//	void WindowResizeEvent(ResizeEvent *event) override;

		private:
			std::shared_ptr<OpenGLTexture> texFunc;
			std::vector<ysl::RGBASpectrum> funcData;
			ImVec2 lastMousePos;
			RGBASpectrum spec;
		};

	}
}
#endif