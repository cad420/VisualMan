
#ifndef _VERTEXATTRIBSETINTERFACE_H_
#define _VERTEXATTRIBSETINTERFACE_H_

namespace vm
{
class AbstraArray;

class IVertexAttribSet
{
public:
	virtual void SetVertexPositionArray( VMRef<AbstraArray> data ) = 0;
	virtual VMRef<AbstraArray> GetVertexArray() = 0;
	virtual VMRef<const AbstraArray> GetVertexArray() const = 0;

	virtual void SetVertexNormalArray( VMRef<AbstraArray> data ) = 0;
	virtual VMRef<AbstraArray> GetNormalArray() = 0;
	virtual VMRef<const AbstraArray> GetNormalArray() const = 0;

	virtual void SetVertexColorArray( VMRef<AbstraArray> data ) = 0;
	virtual VMRef<AbstraArray> GetColorArray() = 0;
	virtual VMRef<const AbstraArray> GetColorArray() const = 0;

	virtual void SetVertexTexCoordArray( VMRef<AbstraArray> data ) = 0;
	virtual VMRef<AbstraArray> GetTexCoordArray() = 0;
	virtual VMRef<const AbstraArray> GetTexCoordArray() const = 0;

	virtual void SetVertexAttribArray( VertexAttribArrayIndexType attribLocation, VMRef<AbstraArray> data ) = 0;
	virtual VMRef<AbstraArray> GetVertexAttribArray( int attribLocation ) = 0;
	virtual VMRef<const AbstraArray> GetVertexAttribArray( int attribLocation ) const = 0;
};

}  // namespace vm

#endif