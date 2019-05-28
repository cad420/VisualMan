
#include "transformchangecallback.h"
#include "camera.h"
#include "shaderprogram.h"
#include "../../lib/gl3w/GL/gl3w.h"
#include "../opengl/openglutils.h"

namespace ysl
{
	namespace  vm
	{
		void TransformChangeCallback::UpdateTransform(const GLSLProgram * program,
			const Camera * camera,
			const Transform* transform)
		{
			// Because there uniforms are volatile every loop. They should not be saved 
			// in a uniform set. we just set them by using the OpenGL api

			// NOTE: program is already be used before this call
			if (program)
			{
				assert(program->Handle());
				//Debug("%s %d\n", __FILE__, __LINE__);

				const int modelLoc = program->GetWorldMatrixUniformLocation();

				if (modelLoc != -1)
				{
					//Debug("%s %d\n", __FILE__, __LINE__);
					if (transform)
					{
						GL(glUniformMatrix4fv(modelLoc, 1, GL_TRUE, transform->Matrix().m[0]));
					}

				}

				const int viewLoc = program->GetViewMatrixUniformLocation();
				if (viewLoc != -1)
				{
					if (camera)
					{
						//Debug("%s %d\n", __FILE__, __LINE__);
						GL(glUniformMatrix4fv(viewLoc, 1, GL_TRUE, camera->ViewMatrix().Matrix().m[0]));
					}

				}

				const int projLoc = program->GetProjectionMatrixUniformLocation();
				if (projLoc != -1)
				{
					if (camera)
					{
						//Debug("%s %d\n", __FILE__, __LINE__);
						GL(glUniformMatrix4fv(projLoc, 1, GL_TRUE, camera->ProjectionMatrix().Matrix().m[0]));
					}
				}

				const int normalLoc = program->GetNormalMatrixUniformLocation();
				if (normalLoc != -1)
				{
					if (camera)
					{
						//Debug("%s %d\n", __FILE__, __LINE__);
						GL(glUniformMatrix3fv(normalLoc, 1, GL_TRUE, camera->ViewMatrix().Matrix().NormalMatrix().FlatData()));
					}
				}

				const int mvpLoc = program->GetMVPUniformLocation();
				if (mvpLoc != -1)
				{
					Matrix4x4 mvp;
					if (camera)
					{
						mvp = mvp * camera->ProjectionMatrix().Matrix();
						mvp = mvp * camera->ViewMatrix().Matrix();
					}
					if (transform)
						mvp = mvp * transform->Matrix();
					//Debug("%s %d\n", __FILE__, __LINE__);
					GL(glUniformMatrix4fv(mvpLoc, 1, GL_TRUE, mvp.FlatData()));
				}
			}
		}
	}
}