#version 430 core

uniform sampler1D texTransfunc;
uniform sampler2DRect texStartPos;
uniform sampler2DRect texEndPos;
uniform sampler2DRect texIntermediateResult;
uniform float step;
uniform float ka;
uniform float kd;
uniform float shininess;
uniform float ks;
uniform vec3 lightdir;
uniform vec3 halfway;
in vec2 textureRectCoord;
out vec4 fragColor;

uniform sampler3D cacheVolume;						// volume block cache
uniform ivec3 totalPageDirSize;						// page dir texture size
uniform ivec3 totalPageTableSize;					// page table texture size
uniform ivec3 pageTableBlockEntrySize;				// page table block entry size
uniform ivec3 volumeDataSizeNoRepeat;				// real volume data size (no repeat)
uniform ivec3 blockDataSizeNoRepeat;				// block data size (no repeat)
uniform ivec3 repeatOffset;							// repeat boarder size


layout(binding = 0, rgba32i) uniform iimage3D pageDirTexutre;
layout(binding = 1, rgba32i) uniform iimage3D pageTableTexture;
layout(binding = 2, rgba32f) uniform image2DRect entryPos;
layout(binding = 3, offset = 0) uniform atomic_uint atomic_count;
layout(binding = 4, offset = 4) uniform atomic_uint atomic_need_to_draw_count;


// keywords buffer shows the read-write feature of the buffer.
layout(std430, binding = 0) buffer HashTable {int blockId[];}hashTable;

layout(std430, binding = 1) buffer MissedBlock{int blockId[];}missedBlock;

vec4 virtualVolumeSample(vec3 samplePos,out bool mapped)
{
	vec4 scalar;
	// address translation
	ivec4 pageTableEntry= imageLoad(pageTableTexture,ivec3(samplePos*totalPageTableSize));
	ivec3 voxelCoord=ivec3(samplePos * volumeDataSizeNoRepeat);
	ivec3 blockOffset = voxelCoord % blockDataSizeNoRepeat;
	if(pageTableEntry.w == 2)		// Unmapped flag
	{
		ivec3 blockCoord = voxelCoord/blockDataSizeNoRepeat;
		int blockId = blockCoord.z * totalPageTableSize.y*totalPageTableSize.x 
				+blockCoord.y * totalPageTableSize.x 
				+blockCoord.x;
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
		vec3 samplePoint = pageTableEntry.xyz + blockOffset + repeatOffset;
		samplePoint = samplePoint/textureSize(cacheVolume,0);
		scalar = texture(cacheVolume,samplePoint);
		mapped = true;
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

	//vec3 rayStart = texture2DRect(texStartPos, textureRectCoord).xyz;
	vec3 rayStart = imageLoad(entryPos,ivec2(textureRectCoord)).xyz;
	vec3 rayEnd = texture2DRect(texEndPos, textureRectCoord).xyz;
	vec3 start2end = rayEnd - rayStart;
	vec4 bg = vec4(0.45f, 0.55f, 0.60f, 1.00f);

	if (start2end.x == 0.0 && start2end.y == 0.0 && start2end.z == 0.0) 
	{
		fragColor = bg; // Background Colors
		discard;
	}

	vec4 color = texture2DRect(texIntermediateResult,textureRectCoord);
	//vec4 color = vec4(0,0,0,0);
	//fragColor = color;
	vec3 direction = normalize(start2end);
	float distance = dot(direction, start2end);
	int steps = int(distance / step);
	vec3 samplePoint;
	for (int i = 0; i < steps; ++i) {
		samplePoint = rayStart + direction * step * (float(i) + 0.5);
		//sample a scalar at samplePoint
		bool mapped;
		vec4 scalar = virtualVolumeSample(samplePoint,mapped);
		if (mapped == false) 
		{
			// If this block is unmaped,
			// store the intermediate rendering result and the entry position
			imageStore(entryPos,ivec2(gl_FragCoord),vec4(samplePoint,0.0));
			fragColor = color;
			discard;
			//discard;
		}
		//vec4 scalar = texture(texVolume, samplePoint);
		vec4 sampledColor = texture(texTransfunc, scalar.r);
		color = color + sampledColor * vec4(sampledColor.aaa, 1.0) * (1.0 - color.a);
		if (color.a > 0.99)
			break;
	}

	imageStore(entryPos,ivec2(gl_FragCoord),vec4(rayEnd ,0.0));

	color = color + vec4(bg.rgb, 0.0) * (1.0 - color.a);
	color.a = 1.0;
	fragColor = color;
}