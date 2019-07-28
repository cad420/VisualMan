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

layout(binding = 2, rgba32f) uniform volatile image2D entryPos;
layout(binding = 3, rgba32f) uniform volatile image2D endPos;
layout(binding = 4, rgba32f) uniform volatile image2DRect interResult;

uniform float step;
// illumination params
//uniform vec3 lightdir;
//uniform vec3 halfway;

uniform float ka;
uniform float kd;
uniform float shininess;
uniform float ks;
uniform mat4 vpl_ModelMatrix;

uniform vec3 fuckPos;
in vec2 screenCoord;
out vec4 fragColor;
// Out-Of-Core uniforms
uniform ivec3 repeatOffset;							// repeat boarder size
uniform int lodNum;
uniform int CUR_LOD;

uniform float fov;

layout(binding = 3) uniform atomic_uint atomic_count[10];		// most 10 lods

layout(std430, binding = 0) buffer HashTable {uint blockId[];}hashTable;
layout(std430, binding = 1) buffer MissedBlock{uint blockId[];}missedBlock;
layout(std430, binding = 2) buffer PageTable{uvec4 pageEntry[];}pageTable;

struct LODInfo
{
	ivec4 pageTableSize;			// offset, considering alignment.
	ivec4 volumeDataSizeNoRepeat;
	ivec4 blockDataSizeNoRepeat;
	int pageTableOffset;			// pageTable Offset,
	int hashBufferOffset;			// offset
	int idBufferOffset;				// offset
	int padding;
};

layout(std140, binding = 3) buffer LODInfoBuffer{LODInfo lod[];}lodInfoBuffer;

vec2 vSize =vec2(1024,768);
float aspectRatio = vSize.x/vSize.y;
float lodTables[10]=
{
0.00000000745f,
0.0000000596f,
0.000000476f,
0.00000381f,
0.0000305175f,
0.0002441f,
0.001953125f,
0.15625f,
0.125f,
1.f
};

float stepTable[7]=
{
	0.00005,
	0.00010,
	0.0002,
	0.0004,
	0.0008,
	0.0016,
	0.0032,

};

//
int evalLOD(vec3 samplePos)
{
	
	float d = length(vec3(vpl_ModelMatrix*vec4(samplePos,1.0))-fuckPos);
	const float fovRadian =  fov * 3.1415926535/180.0;
	const float a = fovRadian*fovRadian*1.33333 / (1024*768*1.0);
	const float r =  (a * (d * step/* + d * step*step + step*step*step/ 3.0*/));
	for(int i = 0 ; i < 10;i++)
	{
		if (r < lodTables[i])
		{
			return clamp(i,0,lodNum-1);
		}
	}
}

vec4 lodColors[7]=
{
vec4(1.0,0,0.0,1.0),
vec4(0.0,1,0.0,1.0),
vec4(0.0,0,1.0,1.0),
vec4(1.0,1.0,0.0,1.0),
vec4(1.0,0.0,1.0,1.0),
vec4(0.0,1.0,1.0,1.0),
vec4(1.0,0.0,0.0,1.0)
};

//int evalLOD(vec3 samplePos)
//{
//	float d = length(vec3(vpl_ModelMatrix*vec4(samplePos,1)) - fuckPos);
//	if(d < 100)return 0;
//	if(d < 300)return 1;
//	if(d < 500)return 2;
//	if(d < 800)return 3;
//	if(d < 1300)return 4;
//	return 4;
//}


#ifdef ILLUMINATION
vec3 N;
#endif

vec4 virtualVolumeSample(vec3 samplePos,in out int curLod,out bool mapped)
{
	vec4 scalar;

	ivec3 pageTableSize = ivec3(lodInfoBuffer.lod[curLod].pageTableSize);

	ivec3 volumeDataSizeNoRepeat =  lodInfoBuffer.lod[curLod].volumeDataSizeNoRepeat.xyz;
	ivec3 blockDataSizeNoRepeat = lodInfoBuffer.lod[curLod].blockDataSizeNoRepeat.xyz;

	// address translation
	//ivec3 entry3DIndex = ivec3(samplePos*volumeDataSizeNoRepeat/vec3(blockDataSizeNoRepeat.xyz*pageTableSize.xyz)*pageTableSize);
	ivec3 entry3DIndex = ivec3(samplePos*pageTableSize);
	uint entryFlatIndex = entry3DIndex.z * pageTableSize.x*pageTableSize.y + entry3DIndex.y * pageTableSize.x + entry3DIndex.x;

	uint pageTableOffset = lodInfoBuffer.lod[curLod].pageTableOffset;
	uvec4 pageTableEntry = pageTable.pageEntry[pageTableOffset+entryFlatIndex];

	vec3 voxelCoord = vec3(samplePos * (volumeDataSizeNoRepeat));
	vec3 blockOffset = ivec3(voxelCoord) % blockDataSizeNoRepeat + fract(voxelCoord);


	if(((pageTableEntry.w) & (0x000f)) == 2)		// Unmapped flag
	{
		// search coarser lod
		uint hashTableOffset =lodInfoBuffer.lod[curLod].hashBufferOffset;
		if(atomicCompSwap(hashTable.blockId[hashTableOffset + entryFlatIndex],0,1) == 0)
		{
		    uint index = atomicCounterIncrement(atomic_count[curLod]);

			uint idBufferOffset = lodInfoBuffer.lod[curLod].idBufferOffset;
			missedBlock.blockId[idBufferOffset + index] = entryFlatIndex;
			hashTable.blockId[hashTableOffset + entryFlatIndex] = 1;		// exits
		}
		mapped = false;
	}
	else
	{
		int texId = int((pageTableEntry.w  >> 4) & 0xf);
		vec3 samplePoint = pageTableEntry.xyz * (blockDataSizeNoRepeat + 2*repeatOffset) + blockOffset + (repeatOffset);
		if(texId == 0){
			samplePoint = samplePoint/textureSize(cacheVolume0,0);
			scalar = texture(cacheVolume0,samplePoint);
			#ifdef ILLUMINATION
			N.x = (texture(cacheVolume0, samplePoint+vec3(step,0,0)).r - texture(cacheVolume0, samplePoint+vec3(-step,0,0) ).r) ;
			N.y = (texture(cacheVolume0, samplePoint+vec3(0,step,0)).r - texture(cacheVolume0, samplePoint+vec3(0,-step,0) ).r) ;
			N.z = (texture(cacheVolume0, samplePoint+vec3(0,0,step)).r - texture(cacheVolume0, samplePoint+vec3(0,0,-step) ).r) ;
			#endif
		}else if(texId == 1){
			samplePoint = samplePoint/textureSize(cacheVolume1,0);
			scalar = texture(cacheVolume1,samplePoint);
			#ifdef ILLUMINATION
			N.x = (texture(cacheVolume1, samplePoint+vec3(step,0,0)).r - texture(cacheVolume1, samplePoint+vec3(-step,0,0) ).r) ;
			N.y = (texture(cacheVolume1, samplePoint+vec3(0,step,0)).r - texture(cacheVolume1, samplePoint+vec3(0,-step,0) ).r) ;
			N.z = (texture(cacheVolume1, samplePoint+vec3(0,0,step)).r - texture(cacheVolume1, samplePoint+vec3(0,0,-step) ).r) ;
			#endif
		}else if(texId == 2){
			samplePoint = samplePoint/textureSize(cacheVolume2,0);
			scalar = texture(cacheVolume2,samplePoint);
			#ifdef ILLUMINATION
			N.x = (texture(cacheVolume2, samplePoint+vec3(step,0,0)).r - texture(cacheVolume2, samplePoint+vec3(-step,0,0) ).r) ;
			N.y = (texture(cacheVolume2, samplePoint+vec3(0,step,0)).r - texture(cacheVolume2, samplePoint+vec3(0,-step,0) ).r) ;
			N.z = (texture(cacheVolume2, samplePoint+vec3(0,0,step)).r - texture(cacheVolume2, samplePoint+vec3(0,0,-step) ).r) ;
			#endif
		}else if(texId == 3){
			samplePoint = samplePoint/textureSize(cacheVolume3,0);
			scalar = texture(cacheVolume3,samplePoint);
			#ifdef ILLUMINATION
			N.x = (texture(cacheVolume3, samplePoint+vec3(step,0,0)).r - texture(cacheVolume3, samplePoint+vec3(-step,0,0) ).r) ;
			N.y = (texture(cacheVolume3, samplePoint+vec3(0,step,0)).r - texture(cacheVolume3, samplePoint+vec3(0,-step,0) ).r) ;
			N.z = (texture(cacheVolume3, samplePoint+vec3(0,0,step)).r - texture(cacheVolume3, samplePoint+vec3(0,0,-step) ).r) ;
			#endif
		}
		mapped = true;
	}
	return scalar;
}

#ifdef ILLUMINATION
vec3 PhongShadingEx(vec3 diffuseColor)
{
	vec3 shadedValue = vec3(0, 0, 0);
	//N = N * 2.0 - 1.0;
	N = -normalize(N);
	vec3 L = normalize(lightdir);
	vec3 H = normalize(halfway);
	//specularcolor
	//vec3 H = normalize(V+L);
	float NdotH = pow(max(dot(N, H), 0.0), shininess);
	float NdotL = max(dot(N, L), 0.0);
	vec3 ambient = 0.1 * diffuseColor.rgb;
	vec3 specular = 0.1 * NdotH * vec3(1.0, 1.0, 1.0);
	vec3 diffuse = vec3(0);1.0 * NdotL * diffuseColor.rgb;
	shadedValue = specular + diffuse + ambient;
	return shadedValue;
}
#endif




void main()
{

	vec3 rayStart = imageLoad(entryPos,ivec2(gl_FragCoord)).xyz;
	vec3 rayEnd = imageLoad(endPos,ivec2(gl_FragCoord)).xyz;
	vec3 start2end = rayEnd - rayStart;
	vec4 color = imageLoad(interResult,ivec2(gl_FragCoord));

	vec3 direction = normalize(start2end);
	float distance = dot(direction, start2end);
	int steps = int(distance / step);
	vec3 samplePoint = rayStart;


	for (int i = 0; i < steps; ++i) 
	{
		//int curLod = evalLOD(samplePoint);
		int curLod =0;

		samplePoint = rayStart + direction * stepTable[curLod] * (float(i) + 0.5);

		if(samplePoint.x <= 0.01 ||
		samplePoint.y <= 0.01 ||
		samplePoint.z <= 0.01 || 
		samplePoint.x >= 0.99 ||
		samplePoint.y >= 0.99 || 
		samplePoint.z >= 0.99)
		continue;
		//sample a scalar at samplePoint
		bool mapped = true;
		vec4 scalar = virtualVolumeSample(samplePoint,curLod,mapped);
		if (mapped == false) 
		{
			imageStore(entryPos,ivec2(gl_FragCoord),vec4(samplePoint,0.0));
			memoryBarrier();
			imageStore(interResult,ivec2(gl_FragCoord),vec4(color));
			memoryBarrier();
			discard;
		}

		//return;
		vec4 sampledColor = texture(texTransfunc, scalar.r);
		#ifdef ILLUMINATION
		sampledColor.rgb = PhongShadingEx(sampledColor.rgb);
		#endif
		color = color + lodColors[curLod]*sampledColor * vec4(sampledColor.aaa, 1.0) * (1.0 - color.a);
		//color = color + vec4(normalize(N),1.0) * vec4(sampledColor.aaa, 1.0) * (1.0 - color.a);
		if (color.a > 0.99)
		{
			break;
		}
	}
	imageStore(entryPos,ivec2(gl_FragCoord),vec4(rayEnd ,0.0));		// Terminating flag
	memoryBarrier();
	//vec4 bg = vec4(0.45f, 0.55f, 0.60f, 1.00f);
	vec4 bg = vec4(1.f,1.f,1.f, 1.00f);
	color = color + vec4(bg.rgb, 0.0) * (1.0 - color.a);
	color.a = 1.0;
	fragColor = color;
}
