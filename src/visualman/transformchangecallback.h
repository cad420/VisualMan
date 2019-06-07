
#ifndef _TRANSFORMCHANGECALLBACK_H_
#define _TRANSFORMCHANGECALLBACK_H_


namespace ysl 
{
	class Transform;
}

namespace ysl
{
	namespace vm
	{
		class GLSLProgram;
		class Camera;

		class  TransformChangeCallback
		{
		public:
			virtual void UpdateTransform(
				const GLSLProgram * program,
				const Camera* camera,
				const Transform* transform);
			virtual ~TransformChangeCallback() = default;
		private:

		};

	}
}

#endif
