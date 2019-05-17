
#ifndef _VERTEXATTRIBSETINTERFACE_H_
#define _VERTEXATTRIBSETINTERFACE_H_

namespace ysl
{
	namespace graphics
	{
		class AbstraArray;

		class IVertexAttribSet
		{
		public:
			virtual void SetVertexArray(Ref<AbstraArray> data) = 0;
			virtual Ref<AbstraArray> GetVertexArray() = 0;
			virtual Ref<const AbstraArray> GetVertexArray()const = 0;

			virtual void SetNormalArray(Ref<AbstraArray> data) = 0;
			virtual Ref<AbstraArray> GetNormalArray() = 0;
			virtual Ref<const AbstraArray> GetNormalArray()const = 0;

			virtual void SetColorArray(Ref<AbstraArray> data) = 0;
			virtual Ref<AbstraArray> GetColorArray() = 0;
			virtual Ref<const AbstraArray> GetColorArray()const = 0;

			virtual void SetTexCoordArray(Ref<AbstraArray> data) = 0;
			virtual Ref<AbstraArray> GetTexCoordArray() = 0;
			virtual Ref<const AbstraArray> GetTexCoordArray()const = 0;

			virtual void SetVertexAttribArray(int attribLocation,Ref<AbstraArray> data) = 0;
			virtual Ref<AbstraArray> GetVertexAttribArray(int attribLocation) = 0;
			virtual Ref<const AbstraArray> GetVertexAttribArray(int attribLocation) const= 0;
		};

	}
}

#endif