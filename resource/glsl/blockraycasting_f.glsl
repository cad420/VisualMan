#version 430 core

/**
* This shader is use to implement a out-of-core volume rendering
*/

//#define ILLUMINATION

/**
* texTransfunc: A 1D texture represent the transfer function. OPTIONAL:False
* cacheVolume0: A 3D texture represent the in-memory volume data. OPTIONAL:False
* cacheVolume1: A 3D texture represent the in-memory volume data. OPTIONAL:True
* cacheVolume2: A 3D texture represent the in-memory volume data. OPTIONAL:True
* cacheVolume3: A 3D texture represent the in-memory volume data. OPTIONAL:True
*/

uniform sampler1D texTransfunc;
uniform sampler3D cacheVolume0;
uniform sampler3D cacheVolume1;
uniform sampler3D cacheVolume2;
uniform sampler3D cacheVolume3;
//uniform sampler3D cacheVolumes[5];

layout( binding = 2, rgba32f ) uniform volatile image2D entryPos;
layout( binding = 3, rgba32f ) uniform volatile image2D endPos;
layout( binding = 4, rgba32f ) uniform volatile image2DRect interResult;

uniform float step;
// illumination params
//uniform vec3 lightdir;
//uniform vec3 halfway;

uniform float ka;
uniform float kd;
uniform float shininess;
uniform float ks;
uniform mat4 vpl_ModelMatrix;
uniform mat4 vpl_ViewMatrix;

uniform vec3 fuckPos;
in vec2 screenCoord;
out vec4 fragColor;
// Out-Of-Core uniforms
uniform ivec3 repeatOffset;  // repeat boarder size
uniform int lodNum;
uniform int CUR_LOD;

uniform float fov;

layout( binding = 3 ) uniform atomic_uint atomic_count[ 10 ];  // most 10 lods

layout( std430, binding = 0 ) buffer HashTable
{
	uint blockId[];
}
hashTable;
layout( std430, binding = 1 ) buffer MissedBlock
{
	uint blockId[];
}
missedBlock;
layout( std430, binding = 2 ) buffer PageTable
{
	uvec4 pageEntry[];
}
pageTable;

struct LODInfo
{
	ivec4 pageTableSize;  // offset, considering alignment.
	ivec4 volumeDataSizeNoRepeat;
	ivec4 blockDataSizeNoRepeat;
	int pageTableOffset;   // pageTable Offset,
	int hashBufferOffset;  // offset
	int idBufferOffset;	// offset
	int padding;
};

layout( std140, binding = 3 ) buffer LODInfoBuffer
{
	LODInfo lod[];
}
lodInfoBuffer;

vec2 vSize = vec2( 1024, 768 );
float aspectRatio = vSize.x / vSize.y;

//float stepTable[ 7 ] = {
//	0.000018,
//	0.000036,
//	0.000072,
//	0.000144,
//	0.000288,
//	0.000576,
//	0.001152
//};

float stepTable[ 7 ] = {
	0.00000016,
	0.00000032,
	0.00000064,
	0.000000128,
	0.000000256,
	0.000000512,
	0.000001024
};

//
int EvalLOD( float d )
{
	int lod;
	if ( d < 1500 )
		lod = 0;
	else if ( d < 3000)
		lod = 1;
	else if ( d < 6000)
		lod = 2;
	else if ( d < 8000)
		lod = 3;
	else if ( d < 10000)
		lod = 4;
	else
		lod = 5;
	return clamp( lod, 0, lodNum - 1 );

	//const float fovRadian =  fov * 3.1415926535/180.0;
	//const float a = fovRadian*fovRadian*1.33333 / (1024*768*1.0);
	//const float r =  (a * (d * step + d * step*step + step*step*step/ 3.0));
	//for(int i = 0 ; i < 10;i++)
	//{
	//	if (r < lodTables[i])
	//	{
	//		return clamp(i,0,lodNum-1);
	//	}
	//}
}

vec4 lodColors[ 7 ] = {
	vec4( 1, 0, 0, 0.005 ),	//red
	vec4( 0, 1, 0, 0.005 ),	// green
	vec4( 0, 0, 1, 0.005 ),	// blue
	vec4( 1, 1.0, 0, 0.005 ),  // yellow
	vec4( 1, 0.0, 1, 0.005 ),  // purple
	vec4( 0, 1.0, 1, 0.005 ),  // blue+
	vec4( 1, 0.0, 0, 0.005 )   //
};

float correlation[ 7 ] = {
	1,
	2,
	4,
	8,
	16,
	32,
	64
};

#ifdef ILLUMINATION
vec3 N;
#endif

float EvalDistanceFromViewToBlockCenterCoord( vec3 samplePos, int curLod )
{
	ivec3 pageTableSize = ivec3( lodInfoBuffer.lod[ curLod ].pageTableSize );

	ivec3 volumeDataSizeNoRepeat = lodInfoBuffer.lod[ curLod ].volumeDataSizeNoRepeat.xyz;
	ivec3 blockDataSizeNoRepeat = lodInfoBuffer.lod[ curLod ].blockDataSizeNoRepeat.xyz;

	// address translation
	ivec3 entry3DIndex = ivec3( samplePos * volumeDataSizeNoRepeat / vec3( blockDataSizeNoRepeat.xyz * pageTableSize.xyz ) * pageTableSize );

	vec4 center;
	center.w = 1;
	center.xyz = ( vec3( entry3DIndex ) + vec3( 0.5, 0.5, 0.5 ) ) / vec3(pageTableSize);
	vec4 r = vpl_ModelMatrix * center;
	return distance( fuckPos, r.xyz/r.w );
}

/* Debug Code
uint EvalBlockID( vec3 samplePos,int curLod )
{
	ivec3 pageTableSize = ivec3( lodInfoBuffer.lod[ curLod ].pageTableSize );

	ivec3 volumeDataSizeNoRepeat = lodInfoBuffer.lod[ curLod ].volumeDataSizeNoRepeat.xyz;
	ivec3 blockDataSizeNoRepeat = lodInfoBuffer.lod[ curLod ].blockDataSizeNoRepeat.xyz;

	// address translation
	ivec3 entry3DIndex = ivec3( samplePos * volumeDataSizeNoRepeat / vec3( blockDataSizeNoRepeat.xyz * pageTableSize.xyz ) * pageTableSize );
	//ivec3 entry3DIndex = ivec3(samplePos*pageTableSize);
	uint entryFlatIndex = entry3DIndex.z * pageTableSize.x * pageTableSize.y + entry3DIndex.y * pageTableSize.x + entry3DIndex.x;

	return entryFlatIndex;
}

vec3 EvalBlockColor(vec3 samplePos,int curLod )
{
	ivec3 pageTableSize = ivec3( lodInfoBuffer.lod[ curLod ].pageTableSize );

	ivec3 volumeDataSizeNoRepeat = lodInfoBuffer.lod[ curLod ].volumeDataSizeNoRepeat.xyz;
	ivec3 blockDataSizeNoRepeat = lodInfoBuffer.lod[ curLod ].blockDataSizeNoRepeat.xyz;

	// address translation
	ivec3 entry3DIndex = ivec3( samplePos * volumeDataSizeNoRepeat / vec3( blockDataSizeNoRepeat.xyz * pageTableSize.xyz ) * pageTableSize );
	return vec3(entry3DIndex)/pageTableSize;
}
*/

vec4 virtualVolumeSample( vec3 samplePos, in out int curLod, out bool mapped, out vec3 blockColor )
{
	vec4 scalar;

	ivec3 pageTableSize = ivec3( lodInfoBuffer.lod[ curLod ].pageTableSize );

	ivec3 volumeDataSizeNoRepeat = lodInfoBuffer.lod[ curLod ].volumeDataSizeNoRepeat.xyz;
	ivec3 blockDataSizeNoRepeat = lodInfoBuffer.lod[ curLod ].blockDataSizeNoRepeat.xyz;

	// address translation
	ivec3 entry3DIndex = ivec3( samplePos * volumeDataSizeNoRepeat / vec3( blockDataSizeNoRepeat.xyz * pageTableSize.xyz ) * pageTableSize );
	//ivec3 entry3DIndex = ivec3(samplePos*pageTableSize);
	uint entryFlatIndex = entry3DIndex.z * pageTableSize.x * pageTableSize.y + entry3DIndex.y * pageTableSize.x + entry3DIndex.x;

	blockColor = vec3( entry3DIndex ) / pageTableSize;

	uint pageTableOffset = lodInfoBuffer.lod[ curLod ].pageTableOffset;
	uvec4 pageTableEntry = pageTable.pageEntry[ pageTableOffset + entryFlatIndex ];

	vec3 voxelCoord = vec3( samplePos * ( volumeDataSizeNoRepeat ) );
	vec3 blockOffset = ivec3( voxelCoord ) % blockDataSizeNoRepeat + fract( voxelCoord );

	if ( ( ( pageTableEntry.w ) & ( 0x000f ) ) == 2 )  // Unmapped flag
	{
		// search coarser lod
		uint hashTableOffset = lodInfoBuffer.lod[ curLod ].hashBufferOffset;
		if ( atomicCompSwap( hashTable.blockId[ hashTableOffset + entryFlatIndex ], 0, 1 ) == 0 ) {
			uint index = atomicCounterIncrement( atomic_count[ curLod ] );

			uint idBufferOffset = lodInfoBuffer.lod[ curLod ].idBufferOffset;
			missedBlock.blockId[ idBufferOffset + index ] = entryFlatIndex;
			hashTable.blockId[ hashTableOffset + entryFlatIndex ] = 1;  // exits
		}
		mapped = false;
	} else {
		int texId = int( ( pageTableEntry.w >> 4 ) & 0xf );
		vec3 samplePoint = pageTableEntry.xyz * ( blockDataSizeNoRepeat + 2 * repeatOffset ) + blockOffset + ( repeatOffset );
		if ( texId == 0 ) {
			samplePoint = samplePoint / textureSize( cacheVolume0, 0 );
			scalar = texture( cacheVolume0, samplePoint );
#ifdef ILLUMINATION
			N.x = ( texture( cacheVolume0, samplePoint + vec3( step, 0, 0 ) ).r - texture( cacheVolume0, samplePoint + vec3( -step, 0, 0 ) ).r );
			N.y = ( texture( cacheVolume0, samplePoint + vec3( 0, step, 0 ) ).r - texture( cacheVolume0, samplePoint + vec3( 0, -step, 0 ) ).r );
			N.z = ( texture( cacheVolume0, samplePoint + vec3( 0, 0, step ) ).r - texture( cacheVolume0, samplePoint + vec3( 0, 0, -step ) ).r );
#endif
		} else if ( texId == 1 ) {
			samplePoint = samplePoint / textureSize( cacheVolume1, 0 );
			scalar = texture( cacheVolume1, samplePoint );
#ifdef ILLUMINATION
			N.x = ( texture( cacheVolume1, samplePoint + vec3( step, 0, 0 ) ).r - texture( cacheVolume1, samplePoint + vec3( -step, 0, 0 ) ).r );
			N.y = ( texture( cacheVolume1, samplePoint + vec3( 0, step, 0 ) ).r - texture( cacheVolume1, samplePoint + vec3( 0, -step, 0 ) ).r );
			N.z = ( texture( cacheVolume1, samplePoint + vec3( 0, 0, step ) ).r - texture( cacheVolume1, samplePoint + vec3( 0, 0, -step ) ).r );
#endif
		} else if ( texId == 2 ) {
			samplePoint = samplePoint / textureSize( cacheVolume2, 0 );
			scalar = texture( cacheVolume2, samplePoint );
#ifdef ILLUMINATION
			N.x = ( texture( cacheVolume2, samplePoint + vec3( step, 0, 0 ) ).r - texture( cacheVolume2, samplePoint + vec3( -step, 0, 0 ) ).r );
			N.y = ( texture( cacheVolume2, samplePoint + vec3( 0, step, 0 ) ).r - texture( cacheVolume2, samplePoint + vec3( 0, -step, 0 ) ).r );
			N.z = ( texture( cacheVolume2, samplePoint + vec3( 0, 0, step ) ).r - texture( cacheVolume2, samplePoint + vec3( 0, 0, -step ) ).r );
#endif
		} else if ( texId == 3 ) {
			samplePoint = samplePoint / textureSize( cacheVolume3, 0 );
			scalar = texture( cacheVolume3, samplePoint );
#ifdef ILLUMINATION
			N.x = ( texture( cacheVolume3, samplePoint + vec3( step, 0, 0 ) ).r - texture( cacheVolume3, samplePoint + vec3( -step, 0, 0 ) ).r );
			N.y = ( texture( cacheVolume3, samplePoint + vec3( 0, step, 0 ) ).r - texture( cacheVolume3, samplePoint + vec3( 0, -step, 0 ) ).r );
			N.z = ( texture( cacheVolume3, samplePoint + vec3( 0, 0, step ) ).r - texture( cacheVolume3, samplePoint + vec3( 0, 0, -step ) ).r );
#endif
		}
		mapped = true;
	}
	return scalar;
}

#ifdef ILLUMINATION
vec3 PhongShadingEx( vec3 diffuseColor )
{
	vec3 shadedValue = vec3( 0, 0, 0 );
	//N = N * 2.0 - 1.0;
	N = -normalize( N );
	vec3 L = normalize( lightdir );
	vec3 H = normalize( halfway );
	//specularcolor
	//vec3 H = normalize(V+L);
	float NdotH = pow( max( dot( N, H ), 0.0 ), shininess );
	float NdotL = max( dot( N, L ), 0.0 );
	vec3 ambient = 0.1 * diffuseColor.rgb;
	vec3 specular = 0.1 * NdotH * vec3( 1.0, 1.0, 1.0 );
	vec3 diffuse = vec3( 0 );
	1.0 * NdotL *diffuseColor.rgb;
	shadedValue = specular + diffuse + ambient;
	return shadedValue;
}
#endif

bool isboader( vec3 point )
{
	const float eps = 0.003;
	float x = point.x;
	float y = point.y;
	float z = point.z;
	if ( ( abs( x ) < eps && abs( y ) <= eps ) || ( abs( x ) < eps && abs( y - 1 ) <= eps ) || ( abs( x - 1 ) < eps && abs( y ) <= eps ) || ( abs( x - 1 ) < eps && abs( y - 1 ) <= eps ) || ( abs( x ) < eps && abs( z ) <= eps ) || ( abs( x ) < eps && abs( z - 1 ) <= eps ) || ( abs( x - 1 ) < eps && abs( z ) <= eps ) || ( abs( x - 1 ) < eps && abs( z - 1 ) <= eps ) || ( abs( z ) < eps && abs( y ) <= eps ) || ( abs( z ) < eps && abs( y - 1 ) <= eps ) || ( abs( z - 1 ) < eps && abs( y ) <= eps ) || ( abs( z - 1 ) < eps && abs( y - 1 ) <= eps ) )
		return true;
	else
		return false;
}

// A pseudorandom genrator
float rand( vec2 co )
{
	return fract( sin( dot( co.xy, vec2( 12.9898, 78.233 ) ) ) * 43758.5453 );
}

void main()
{
	vec4 rayStartInfo = imageLoad( entryPos, ivec2( gl_FragCoord ) ).xyzw;
	vec3 rayStart = rayStartInfo.xyz;
	vec3 rayEnd = imageLoad( endPos, ivec2( gl_FragCoord ) ).xyz;
	vec3 start2end = rayEnd - rayStart;
	vec4 color = imageLoad( interResult, ivec2( gl_FragCoord ) );
	vec4 bg = vec4( 0.f, 0.f, 0.f, .00f );
	vec3 direction = normalize( start2end );
	float distance = dot( direction, start2end );
	int steps = int( distance / step );
	vec3 samplePoint = rayStart;		  //+  direction*0.001;
	int prevLOD = int( rayStartInfo.w );  // & 0xf;
										  //if(int((int(rayStartInfo.w) >> 4) & 0xf) == 0)
	//{
	//  samplePoint += direction * 0.001;
	// }

	vec4 boundingBoxColor = vec4( 1, 0, 0, 1 );

	//	if(start2end.x == 0 && start2end.y == 0 && start2end.z == 0)
	//	{
	//		fragColor = bg;
	//		return;
	//	}

	for ( int i = 0; i < 10000; ++i ) {
		if ( samplePoint.x < 0.00 ||
			 samplePoint.y < 0.00 ||
			 samplePoint.z < 0.00 ||
			 samplePoint.x > 1.0 ||
			 samplePoint.y > 1.0 ||
			 samplePoint.z > 1.0 )
			break;

		int curLod = EvalLOD( EvalDistanceFromViewToBlockCenterCoord( samplePoint, prevLOD ) );

		samplePoint += direction * stepTable[ curLod ] * ( float( i ) + 0.5 );

		//if(isboader(samplePoint) == true)
		//	color = color + boundingBoxColor * vec4(boundingBoxColor.aaa, 1.0) * (1.0 - color.a);

		//sample a scalar at samplePoint

		bool mapped = true;
		vec3 blockColor;
		vec4 scalar = virtualVolumeSample( samplePoint, curLod, mapped, blockColor );
		if ( mapped == false ) {
			imageStore( entryPos, ivec2( gl_FragCoord ), vec4( samplePoint, float( curLod ) ) );
			imageStore( interResult, ivec2( gl_FragCoord ), vec4( color ) );
			discard;
		}

		//vec4 sampledColor = texture(texTransfunc, scalar.r);
		float alpha = 0.03, a = 1.0;
		vec4 sampledColor;
		float x = ( scalar.r - alpha ) / ( a - alpha );
		if ( scalar.r < alpha )
			sampledColor.a = 0;
		else if ( scalar.r > a )
			sampledColor = vec4( 1, 1, 1, 1 );
		else
			sampledColor = vec4( x, x, x, x );
		//sampledColor.a = 1-pow((1-sampledColor.a),correlation[curLod]);
		color = color + sampledColor * vec4( sampledColor.aaa, 1.0 ) * ( 1.0 - color.a );
		//vec4 b = vec4(blockColor,0.001);
		//color = color + b * vec4(b.aaa, 1.0) * (1.0 - color.a);
		//color.a = color.a + (1-color.a) * sampledColor.a;

		// Debug Code
		/*
		//vec4 blockColor = lodColors[curLod];
        //blockColor.a = 1;
		//blockColor.rgb = EvalBlockColor(samplePoint,0);
		//color = color + blockColor * vec4( blockColor.aaa, 1.0 ) * ( 1.0 - color.a );
		//color.a = color.a + ( 1 - color.a ) * blockColor.a;
*/
		if ( color.a > 0.99 ) {
			break;
		}
	}

	imageStore( entryPos, ivec2( gl_FragCoord ), vec4( rayEnd, 0.0 ) );  // Terminating flag
	//memoryBarrier();
	//vec4 bg = vec4(0.45f, 0.55f, 0.60f, 1.00f);
	color = color + vec4( bg.rgb, 0.0 ) * ( 1.0 - color.a );
	color.a = 1.0;
	fragColor = color;
}
