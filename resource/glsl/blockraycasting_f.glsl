#version 430 core

//
uniform sampler1D texTransfunc;
//uniform sampler2D texStartPos;
//uniform sampler2D texEndPos;
//uniform sampler3D texVolume;
uniform sampler3D cacheVolume;
layout(binding = 1, rgba32ui) uniform volatile uimage3D pageTableTexture;
uniform ivec3 totalPageTableSize;					// page table texture size1
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
//layout(binding = 0, rgba32i) uniform volatile iimage3D pageDirTexutre;
layout(binding = 3, offset = 0) uniform atomic_uint atomic_count;
// keywords buffer shows the read-write feature of the buffer.
layout(std430, binding = 0) buffer HashTable {int blockId[];}hashTable;
layout(std430, binding = 1) buffer MissedBlock{int blockId[];}missedBlock;


vec4 virtualVolumeSample(vec3 samplePos,out bool mapped)
{
	vec4 scalar;
	//ivec3 totalPageTableSize = imageSize(pageTableTexture);
	// address translation
	vec4 pageTableEntry= imageLoad(pageTableTexture,ivec3(samplePos*totalPageTableSize));
	vec3 voxelCoord=vec3(samplePos * (volumeDataSizeNoRepeat));
	vec3 blockOffset = ivec3(voxelCoord) % blockDataSizeNoRepeat + fract(voxelCoord);


	ivec3 blockCoord = ivec3(voxelCoord / blockDataSizeNoRepeat);
	int blockId = blockCoord.z * totalPageTableSize.y*totalPageTableSize.x 
				+blockCoord.y * totalPageTableSize.x 
				+blockCoord.x;
	if(pageTableEntry.w == 2)		// Unmapped flag
	{
		if(atomicCompSwap(hashTable.blockId[blockId],0,1) == 0)
		{
			uint index = atomicCounterIncrement(atomic_count);
			missedBlock.blockId[index] = blockId;
			hashTable.blockId[blockId] = 1;		// exits
		}
		mapped = false;
	}
	else 
	{
		vec3 samplePoint = pageTableEntry.xyz * (blockDataSizeNoRepeat + 2*repeatOffset) + blockOffset + (repeatOffset);
		samplePoint = samplePoint/textureSize(cacheVolume,0);
		scalar = texture(cacheVolume,samplePoint);
		mapped = true;
		//return vec4(0.8+blockId*0.005,0.9,0.8,0.8);
	}
	return scalar;
}

vec3 PhongShadingEx(vec3 samplePos, vec3 diffuseColor)
{
	vec3 shadedValue = vec3(0, 0, 0);
	//virtualVolumeSample(vec3 samplePos,out bool mapped)
	vec3 N;
//	N.x = (texture(texVolume, samplePos+vec3(step,0,0) ).w - texture(texVolume, samplePos+vec3(-step,0,0) ).w) - 1.0;
//	N.y = (texture(texVolume, samplePos+vec3(0,step,0) ).w - texture(texVolume, samplePos+vec3(0,-step,0) ).w) - 1.0;
//	N.z = (texture(texVolume, samplePos+vec3(0,0,step) ).w - texture(texVolume, samplePos+vec3(0,0,-step) ).w) - 1.0;
	bool placeHolder;
	N.x = (virtualVolumeSample(samplePos+vec3(step,0,0) ,placeHolder).w - virtualVolumeSample(samplePos+vec3(-step,0,0),placeHolder ).w) - 1.0;
	N.y = (virtualVolumeSample(samplePos+vec3(0,step,0) ,placeHolder).w - virtualVolumeSample(samplePos+vec3(0,-step,0) ,placeHolder).w) - 1.0;
	N.z = (virtualVolumeSample(samplePos+vec3(0,0,step) ,placeHolder).w - virtualVolumeSample(samplePos+vec3(0,0,-step) ,placeHolder).w) - 1.0;
	N = N * 2.0 - 1.0;
	N = -normalize(N);
	vec3 L = lightdir;
	vec3 H = halfway;
	//specularcolor
	//vec3 H = normalize(V+L);
	float NdotH = pow(max(dot(N, H), 0.0), shininess);
	float NdotL = max(dot(N, L), 0.0);
	vec3 ambient = ka * diffuseColor.rgb;
	vec3 specular = ks * NdotH * vec3(1.0, 1.0, 1.0);
	vec3 diffuse = kd * NdotL * diffuseColor.rgb;
	shadedValue = specular + diffuse + ambient;
	return shadedValue;
}

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
		vec4 sampledColor = texture(texTransfunc, scalar.r);
		color = color + sampledColor * vec4(sampledColor.aaa, 1.0) * (1.0 - color.a);
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
}
