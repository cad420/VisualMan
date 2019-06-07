#ifndef _VMQTWINDOW_H_
#define _VMQTWINDOW_H_

#include <QOpenGLWidget>
#include "rendercontext.h"
#include "graphictype.h"

//#define QT_NO_OPENGL

namespace ysl
{
	namespace vm
	{
		class VISUALMAN_EXPORT_IMPORT VMQtWindow:public QOpenGLWidget, public RenderContext
		{
		public:
			VMQtWindow(QWidget * parent = nullptr);
			void initializeGL() override;
			void resizeGL(int w, int h) override;
			void paintGL() override;
			void MakeCurrent() override;
			void SwapBuffer() override;
			void Update() override;
		protected:
			void mousePressEvent(QMouseEvent* event) override;
			void mouseMoveEvent(QMouseEvent* event) override;
			void mouseReleaseEvent(QMouseEvent* event) override;
		};
	}
}

#endif
