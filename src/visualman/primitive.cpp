
#include "primitive.h"
#include "rendercontext.h"
#include "abstraarray.h"
#include "graphictype.h"
#include <VMGraphics/objreader.h>
#include <cassert>
#include <iostream>

#include "drawelements.h"
#include "abstrdraw.h"

namespace ysl
{
namespace vm
{
Primitive::~Primitive()
{
	DestroyVAO();
}

void Primitive::SetVertexPositionArray( Ref<AbstraArray> data )
{
	//if (vertexAttribArrays[VA_VertexPositionAttrib] == data)
	//	return;
	//vertexAttribArrays[VA_VertexPositionAttrib] = std::move(data);
	//bind2VAO(VA_VertexPositionAttrib);
	//boundToVAO[VA_VertexPositionAttrib] = false;
	SetVertexAttribArray( VA_VertexPositionAttrib, std::move( data ) );
}

void Primitive::SetVertexNormalArray( Ref<AbstraArray> data )
{
	//if (vertexAttribArrays[VA_VertexNormalAttrib] == data)
	//	return;
	//vertexAttribArrays[VA_VertexNormalAttrib] = std::move(data);
	//bind2VAO(VA_VertexNormalAttrib);
	//boundToVAO[VA_VertexNormalAttrib] = false;
	SetVertexAttribArray( VA_VertexNormalAttrib, std::move( data ) );
}

void Primitive::SetVertexColorArray( Ref<AbstraArray> data )
{
	//if (vertexAttribArrays[VA_VertexColorAttrib] == data)
	//	return;
	//vertexAttribArrays[VA_VertexColorAttrib] = std::move(data);
	//bind2VAO(VA_VertexColorAttrib);
	//boundToVAO[VA_VertexColorAttrib] = false;
	SetVertexAttribArray( VA_VertexColorAttrib, std::move( data ) );
}

void Primitive::SetVertexTexCoordArray( Ref<AbstraArray> data )
{
	//if (vertexAttribArrays[VA_VertexTexCoordAttrib] == data)
	//	return;
	//vertexAttribArrays[VA_VertexTexCoordAttrib] = std::move(data);
	//bind2VAO(VA_VertexTexCoordAttrib);
	//boundToVAO[VA_VertexTexCoordAttrib] = false;
	SetVertexAttribArray( VA_VertexTexCoordAttrib, std::move( data ) );
}

void Primitive::SetVertexAttribArray( VertexAttribArrayIndexType attribLocation, Ref<AbstraArray> data )
{
	if ( vertexAttribArrays[ attribLocation ] == data )
		return;
	vertexAttribArrays[ attribLocation ] = std::move( data );
	//bind2VAO(attribLocation);
	boundToVAO[ attribLocation ] = false;  // delayed binding
	vaoCompletion = false;				   // vao need to be rebound because vertex buffer has changed
}

void Primitive::UpdateDirtyBufferObject( BufferObjectUpdateMode mode )
{
	// check any buffer object that need to be update
	const bool discard = mode & VM_BUF_DiscardRAM;
	for ( int i = 0; i < VA_VertexAttribArray_Count; i++ ) {
		if ( vertexAttribArrays[ i ] && vertexAttribArrays[ i ]->IsBufferObjectDataDirty() ) {
			// Is it possible that a vertex attribute array don't have a buffer object?
			// and what it mean?
			//std::cout << i << std::endl;
			assert( vertexAttribArrays[ i ]->GetBufferObject() );
			vertexAttribArrays[ i ]->GetBufferObject()->ReallocBufferData( BU_STATIC_DRAW, discard );
			vertexAttribArrays[ i ]->SetbufferObjectDataDirty( false );
		}
	}

	// Update Draw Call
	for ( const auto &dc : drawCalls ) {
		dc->UpdateDirtyBufferObject( mode );
	}
}

void Primitive::Render_Implement( const Actor *actor, const Shading *shading, const Camera *camera, RenderContext *context ) const
{
	// Bind vbo interface to context
	// A primitive is an implementation of the VertexArray Interface
	// call draw command

	assert( context );

	// Warning: This will modified some record field of vao dirty state
	if ( !IsVAOCompletion() )
		const_cast<Primitive *>( this )->rebind2VAO();

	context->Bind_VAO( vaoHandle );

	//context->BindVertexArray(this);

	for ( const auto &dc : drawCalls ) {
		dc->Render();
	}
}

void Primitive::DestroyVAO()
{
	if ( vaoHandle ) {
		GL( glDeleteVertexArrays( 1, &vaoHandle ) );
		vaoHandle = 0;
		vaoCompletion = false;
		for ( int i = 0; i < VA_VertexAttribArray_Count; i++ ) {
			boundToVAO[ i ] = false;
		}
	}
}

void Primitive::bind2VAO( int attribLocation )
{
	if ( vaoHandle == 0 ) {
		GL( glGenVertexArrays( 1, &vaoHandle ) );
		assert( vaoHandle );
	}
	assert( vertexAttribArrays[ attribLocation ] );

	GL( glBindVertexArray( vaoHandle ) );
	GL( glBindBuffer( GL_ARRAY_BUFFER, vertexAttribArrays[ attribLocation ]->GetBufferObject()->Handle() ) );
	GL( glVertexAttribPointer( attribLocation,
							   vertexAttribArrays[ attribLocation ]->ComponentNum(),
							   vertexAttribArrays[ attribLocation ]->Type(),
							   GL_FALSE,
							   0, (void *)0 ) );
	GL( glEnableVertexAttribArray( attribLocation ) );
	GL( glBindVertexArray( 0 ) );
}

void Primitive::rebind2VAO()
{
	if ( vaoHandle == 0 ) {
		GL( glGenVertexArrays( 1, &vaoHandle ) );
		assert( vaoHandle );
	}
	GL( glBindVertexArray( vaoHandle ) );
	for ( int i = 0; i < VA_VertexAttribArray_Count; i++ ) {
		if ( boundToVAO[ i ] == false && vertexAttribArrays[ i ] ) {
			GL( glBindBuffer( GL_ARRAY_BUFFER, vertexAttribArrays[ i ]->GetBufferObject()->Handle() ) );
			GL( glVertexAttribPointer( i,
									   vertexAttribArrays[ i ]->ComponentNum(),
									   vertexAttribArrays[ i ]->Type(),
									   GL_FALSE,
									   0, reinterpret_cast<void *>( 0 ) ) );
			GL( glEnableVertexAttribArray( i ) );
			boundToVAO[ i ] = true;
		}
	}
	GL( glBindVertexArray( 0 ) );
	vaoCompletion = true;
}

//void OutOfCorePrimitive::Render_Implement(const Actor* actor, const Shading* shading, const Camera* camera,
//	RenderContext* context) const
//{
//	assert(context);

//	// Warning: This will modify some record field of vao dirty state
//	if (!IsVAOCompletion())
//		const_cast<OutOfCorePrimitive*>(this)->rebind2VAO();

//	context->Bind_VAO(vaoHandle);

//	//context->BindVertexArray(this);

//	for (const auto & dc : drawCalls)
//	{
//		dc->Render();
//	//	callback->OnDrawCallFinishedEvent(dc.get(), actor, shading, camera, context);
//	}

//}

Ref<Primitive> MakePrimitive( const std::string &fileName )
{
	ObjReader objReader( fileName );

	if ( objReader.IsLoaded() == false ) {
		throw std::runtime_error( "can not load .obj file" );
	}

	auto vertex = MakeRef<ArrayFloat3>();
	//vertex->GetBufferObject()->SetBufferData(objReader.GetVertexBytes(),
	//	objReader.GetVertices().data(),
	//	BU_STATIC_DRAW);

	vertex->GetBufferObject()->SetLocalData( (char *)objReader.GetVertices().data(), objReader.GetVertexBytes() );

	//vertex->SetbufferObjectDataDirty(false);

	auto normals = MakeRef<ArrayFloat3>();
	//normals->GetBufferObject()->SetBufferData(objReader.GetNormalBytes(),
	//	objReader.GetNormals().data(),
	//	BU_STATIC_DRAW);

	normals->GetBufferObject()->SetLocalData( (char *)objReader.GetNormals().data(), objReader.GetNormalBytes() );

	//normals->SetbufferObjectDataDirty(false);

	auto vertexIndex = MakeRef<ArrayUInt>();
	//vertexIndex->GetBufferObject()->SetBufferData(objReader.GetVertexIndicesBytes(),
	//	objReader.GetFaceIndices().data(),
	//	BU_STATIC_DRAW);

	vertexIndex->GetBufferObject()->SetLocalData( (char *)objReader.GetFaceIndices().data(), objReader.GetVertexIndicesBytes() );
	//vertexIndex->SetbufferObjectDataDirty(false);

	auto primitive = MakeRef<Primitive>();
	primitive->SetVertexPositionArray( vertex );
	primitive->SetVertexNormalArray( normals );

	auto drawElemUi = MakeRef<DrawElementsUInt>( 1 );
	drawElemUi->SetIndexBuffer( vertexIndex );

	primitive->DrawCalls().push_back( drawElemUi );

	return primitive;
}

Ref<Primitive> MakePrimitive( const float *position, size_t positionCount, const float *normal,
							  size_t normalCount, const unsigned *index, size_t indexCount )
{
	auto vertex = MakeRef<ArrayFloat3>();
	//vertex->GetBufferObject()->SetBufferData(objReader.GetVertexBytes(),
	//	objReader.GetVertices().data(),
	//	BU_STATIC_DRAW);

	vertex->GetBufferObject()->SetLocalData( (char *)position, positionCount * sizeof( float ) * 3 );

	//vertex->SetbufferObjectDataDirty(false);

	auto normals = MakeRef<ArrayFloat3>();
	//normals->GetBufferObject()->SetBufferData(objReader.GetNormalBytes(),
	//	objReader.GetNormals().data(),
	//	BU_STATIC_DRAW);

	normals->GetBufferObject()->SetLocalData( normal, normalCount * sizeof( float ) * 3 );

	//normals->SetbufferObjectDataDirty(false);

	auto vertexIndex = MakeRef<ArrayUInt>();
	//vertexIndex->GetBufferObject()->SetBufferData(objReader.GetVertexIndicesBytes(),
	//	objReader.GetFaceIndices().data(),
	//	BU_STATIC_DRAW);

	vertexIndex->GetBufferObject()->SetLocalData( index, indexCount * sizeof( float ) * 3 );
	//vertexIndex->SetbufferObjectDataDirty(false);

	auto primitive = MakeRef<Primitive>();
	primitive->SetVertexPositionArray( vertex );
	primitive->SetVertexNormalArray( normals );

	auto drawElemUi = MakeRef<DrawElementsUInt>( 1 );
	drawElemUi->SetIndexBuffer( vertexIndex );

	primitive->DrawCalls().push_back( drawElemUi );

	return primitive;
}

Ref<Primitive> MakeCubeLines( const Bound3f &bound )
{
	auto proxyGeometry = MakeRef<Primitive>();

	Point3f points[ 8 ];

	for ( int i = 0; i < 8; i++ ) {
		points[ i ] = bound.Corner( i );
	}

	unsigned int indices[] = { 0, 1, 1, 3, 3, 2, 2, 0, 4, 5, 5, 7, 7, 6, 6, 4, 0, 4, 1, 5, 3, 7, 2, 6 };

	auto vertexIndex = MakeRef<ArrayUInt>();
	vertexIndex->GetBufferObject()->SetLocalData( indices, sizeof( indices ) );
	auto vertexArray = MakeRef<ArrayFloat3>();

	vertexArray->GetBufferObject()->SetLocalData( points, sizeof( points ) );
	proxyGeometry->SetVertexPositionArray( vertexArray );

	auto drawCall = MakeRef<DrawElementsUInt>();
	drawCall->SetPrimitiveType( PT_LINES );

	drawCall->SetIndexBuffer( vertexIndex );
	proxyGeometry->DrawCalls().push_back( drawCall );
	return proxyGeometry;
}

Ref<Primitive> MakeCube( const Bound3f &bound )
{
	auto proxyGeometry = MakeRef<Primitive>();

	Point3f points[ 8 ];

	for ( int i = 0; i < 8; i++ ) {
		points[ i ] = bound.Corner( i );
		std::cout << points[ i ];
	}
	unsigned int indices[] = { 0, 2, 1, 1, 2, 3,
							   4, 5, 6, 5, 7, 6,
							   0, 1, 4, 1, 5, 4,
							   2, 6, 3, 3, 6, 7,
							   0, 4, 2, 2, 4, 6,
							   1, 3, 5, 3, 7, 5 };

	float normals[] = { -1, -1, -1,
						1, -1, -1,
						-1, 1, -1,
						1, 1, -1,
						-1, -1, 1,
						1, -1, 1,
						-1, 1, 1,
						1, 1, 1 };

	auto vertexIndex = MakeRef<ArrayUInt>();
	vertexIndex->GetBufferObject()->SetLocalData( indices, sizeof( indices ) );
	auto vertexArray = MakeRef<ArrayFloat3>();
	auto texCoordArray = MakeRef<ArrayFloat3>();
	auto normalArray = MakeRef<ArrayFloat3>();

	vertexArray->GetBufferObject()->SetLocalData( points, sizeof( points ) );

	texCoordArray->GetBufferObject()->SetLocalData( points, sizeof( points ) );

	normalArray->GetBufferObject()->SetLocalData( normals, sizeof( normals ) );

	proxyGeometry->SetVertexPositionArray( vertexArray );

	proxyGeometry->SetVertexTexCoordArray( texCoordArray );

	proxyGeometry->SetVertexNormalArray( normalArray );

	auto drawCall = MakeRef<DrawElementsUInt>();

	drawCall->SetIndexBuffer( vertexIndex );
	proxyGeometry->DrawCalls().push_back( drawCall );
	return proxyGeometry;
}
}  // namespace vm
}  // namespace ysl
