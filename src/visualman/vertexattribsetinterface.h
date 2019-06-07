
#ifndef _VERTEXATTRIBSETINTERFACE_H_
#define _VERTEXATTRIBSETINTERFACE_H_

namespace ysl
{
	namespace vm
	{
		class AbstraArray;

		class IVertexAttribSet
		{
		public:
			virtual void SetVertexPositionArray(Ref<AbstraArray> data) = 0;
			virtual Ref<AbstraArray> GetVertexArray() = 0;
			virtual Ref<const AbstraArray> GetVertexArray()const = 0;

			virtual void SetVertexNormalArray(Ref<AbstraArray> data) = 0;
			virtual Ref<AbstraArray> GetNormalArray() = 0;
			virtual Ref<const AbstraArray> GetNormalArray()const = 0;

			virtual void SetVertexColorArray(Ref<AbstraArray> data) = 0;
			virtual Ref<AbstraArray> GetColorArray() = 0;
			virtual Ref<const AbstraArray> GetColorArray()const = 0;

			virtual void SetVertexTexCoordArray(Ref<AbstraArray> data) = 0;
			virtual Ref<AbstraArray> GetTexCoordArray() = 0;
			virtual Ref<const AbstraArray> GetTexCoordArray()const = 0;

			virtual void SetVertexAttribArray(VertexAttribArrayIndexType attribLocation, Ref<AbstraArray> data) = 0;
			virtual Ref<AbstraArray> GetVertexAttribArray(int attribLocation) = 0;
			virtual Ref<const AbstraArray> GetVertexAttribArray(int attribLocation) const= 0;
		};

	}
}

#endif