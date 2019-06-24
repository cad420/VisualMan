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


uniform ivec3 pageTableSize;				// page table texture size1
layout(binding = 2, rgba32f) uniform volatile image2D entryPos;
layout(binding = 3, rgba32f) uniform volatile image2D endPos;
layout(binding = 4, rgba32f) uniform volatile image2DRect interResult;

uniform float step;
// illumination params
uniform vec3 lightdir;
uniform vec3 halfway;

uniform float ka;
uniform float kd;
uniform float shininess;
uniform float ks;

in vec2 screenCoord;
out vec4 fragColor;
// Out-Of-Core uniforms
uniform ivec3 volumeDataSizeNoRepeat;				// real volume data size (no repeat)
uniform ivec3 blockDataSizeNoRepeat;				// block data size (no repeat)
uniform ivec3 repeatOffset;							// repeat boarder size
layout(binding = 3, offset = 0) uniform atomic_uint atomic_count;
layout(std430, binding = 0) buffer HashTable {uint blockId[];}hashTable;
layout(std430, binding = 1) buffer MissedBlock{uint blockId[];}missedBlock;
layout(std430, binding = 2) buffer PageTable{uvec4 pageEntry[];}pageTable;
//layout(binding = 1, rgba32ui) uniform volatile uimage3D pageTableTexture;

//uniform int lod;

#ifdef ILLUMINATION
vec3 N;
#endif

uint prevVisitedBlockID;
bool hashPrevious = false;

vec4 virtualVolumeSample(vec3 samplePos,out bool mapped)
{
	vec4 scalar;
	//ivec3 totalPageTableSize = imageSize(pageTableTexture);
	// address translation
	ivec3 entry3DIndex = ivec3(samplePos*pageTableSize);
	uint entryFlatIndex = entry3DIndex.z * pageTableSize.x*pageTableSize.y + entry3DIndex.y * pageTableSize.x + entry3DIndex.x;

	uvec4 pageTableEntry = pageTable.pageEntry[entryFlatIndex];

	//uvec4 pageTableEntry= imageLoad(pageTableTexture,ivec3(samplePos*pageTableSize));

	vec3 voxelCoord=vec3(samplePos * (volumeDataSizeNoRepeat));
	vec3 blockOffset = ivec3(voxelCoord) % blockDataSizeNoRepeat + fract(voxelCoord);

	//ivec3 blockCoord = ivec3(voxelCoord / blockDataSizeNoRepeat);
	//int blockId = blockCoord.z * pageTableSize.y*pageTableSize.x 	+blockCoord.y * pageTableSize.x 	+blockCoord.x;

	if(((pageTableEntry.w) & (0x000f)) == 2)		// Unmapped flag
	{
		//if(hashPrevious == false || prevVisitedBlockID != entryFlatIndex)
	//	{
	//		prevVisitedBlockID = entryFlatIndex;
	//		hashPrevious = true;
			if(atomicCompSwap(hashTable.blockId[entryFlatIndex],0,1) == 0)
			{
				uint index = atomicCounterIncrement(atomic_count);
				missedBlock.blockId[index] = entryFlatIndex;
				hashTable.blockId[entryFlatIndex] = 1;		// exits
			}
	//	}
		mapped = false;
	}
	else
	{
		int texId = int((pageTableEntry.w  >> 4)&0xf);
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
			N.x = (texture(cacheVolume1, samplePoint+vec3(step,0,0)).r - texture(cacheVolume2, samplePoint+vec3(-step,0,0) ).r) ;
			N.y = (texture(cacheVolume1, samplePoint+vec3(0,step,0)).r - texture(cacheVolume2, samplePoint+vec3(0,-step,0) ).r) ;
			N.z = (texture(cacheVolume1, samplePoint+vec3(0,0,step)).r - texture(cacheVolume2, samplePoint+vec3(0,0,-step) ).r) ;
			#endif
		}else if(texId == 3){
			samplePoint = samplePoint/textureSize(cacheVolume3,0);
			scalar = texture(cacheVolume3,samplePoint);
			#ifdef ILLUMINATION
			N.x = (texture(cacheVolume2, samplePoint+vec3(step,0,0)).r - texture(cacheVolume3, samplePoint+vec3(-step,0,0) ).r) ;
			N.y = (texture(cacheVolume2, samplePoint+vec3(0,step,0)).r - texture(cacheVolume3, samplePoint+vec3(0,-step,0) ).r) ;
			N.z = (texture(cacheVolume2, samplePoint+vec3(0,0,step)).r - texture(cacheVolume3, samplePoint+vec3(0,0,-step) ).r) ;
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
	int count = 0;
	vec3 direction = normalize(start2end);
	float distance = dot(direction, start2end);
	int steps = int(distance / step);
	vec3 samplePoint = rayStart;

	for (int i = 0; i < steps; ++i) 
	{
		samplePoint = rayStart + direction * step * (float(i) + 0.5);
		//fragColor = vec4(samplePoint,1.0);
		if(samplePoint.x <= 0.01 ||
		samplePoint.y <= 0.01 ||
		samplePoint.z <= 0.01 || 
		samplePoint.x >= 0.99 ||
		samplePoint.y >= 0.99 || 
		samplePoint.z >= 0.99)
		continue;
		//sample a scalar at samplePoint
		bool mapped;
		vec4 scalar = virtualVolumeSample(samplePoint,mapped);
		//count++;
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
		color = color + sampledColor * vec4(sampledColor.aaa, 1.0) * (1.0 - color.a);
		//color = color + vec4(normalize(N),1.0) * vec4(sampledColor.aaa, 1.0) * (1.0 - color.a);
		if (color.a > 0.99)
		{
			break;
		}
	}
	imageStore(entryPos,ivec2(gl_FragCoord),vec4(rayEnd ,0.0));		// Terminating flag
	memoryBarrier();
	vec4 bg = vec4(0.45f, 0.55f, 0.60f, 1.00f);
	color = color + vec4(bg.rgb, 0.0) * (1.0 - color.a);
	color.a = 1.0;
	fragColor = color;
	//fragColor = vec4(normalize(N),1.0);

}
