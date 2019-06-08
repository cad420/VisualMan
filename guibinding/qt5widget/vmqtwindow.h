#ifndef _VMQTWINDOW_H_
#define _VMQTWINDOW_H_

#include <rendercontext.h>
#include <QOpenGLWidget>

//#define QT_NO_OPENGL
#if defined(_WIN32) && defined(VM_SHARED_LIBRARY)
#ifdef vmqtwindow_EXPORTS
#define VMQTWINDOW_EXPORT_IMPORT __declspec(dllexport)
#else
#define VMQTWINDOW_EXPORT_IMPORT __declspec(dllimport)
#endif
#else
#define VMQTWINDOW_EXPORT_IMPORT
#endif

namespace ysl
{
	namespace vm
	{
		class VMQTWINDOW_EXPORT_IMPORT VMQtWindow:public QOpenGLWidget, public RenderContext
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
