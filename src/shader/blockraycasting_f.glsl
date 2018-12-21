#version 430 core

uniform sampler1D texTransfunc;
uniform sampler2DRect texStartPos;
uniform sampler2DRect texEndPos;
uniform sampler3D texVolume;
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
uniform ivec3 volumeDataSize;						// real volume data size (no repeat)
uniform ivec3 blockDataSize;						// block data size (no repeat)
uniform ivec3 repeatOffset;							// repeat boarder size


layout(binding = 0, rgba32i) uniform iimage3D pageDirTexutre;
layout(binding = 1, rgba32i) uniform iimage3D pageTableTexture;
//layout(binding = 2, r32i) uniform iimage1D cacheMissedTexture;

// keywords buffer shows the read-write feature of the buffer.
layout(std430, binding = 0) buffer HashTable
{
	int blockId[];
}hashTable;

layout(std430, binding = 1) buffer MissedBlock{
	int spinLock;
	int count;
	int blockId[];
}missedBlock;


float virtualVolumeSample(vec3 samplePos,out bool mapped)
{
	ivec3 pDirAddress = ivec3(samplePos*totalPageDirSize);
	ivec4 pageDirEntry = imageLoad(pageDirTexutre,pDirAddress);
	ivec3 pTableAddress = ivec3(pageDirEntry) + ivec3(samplePos*totalPageTableSize) % pageTableBlockEntrySize;
	ivec4 pageTableEntry = imageLoad(pageTableTexture,pTableAddress);

	ivec3 voxelCoord = ivec3(samplePos * volumeDataSize);
	ivec3 blockOffset = voxelCoord % blockDataSize;

	if(pageTableEntry.w == 2)		// Unmapped flag
	{
		//calculate block id
		ivec3 blockCoord = voxelCoord/blockDataSize;
		int blockId = blockCoord.z * totalPageTableSize.y*totalPageTableSize.x 
				+blockCoord.y * totalPageTableSize.x 
				+blockCoord.x;
		int exist = atomicCompSwap(hashTable.blockId[blockId],0,1);
		if(exist == 0)
		{
			// lock
			int expect=0;
			while(atomicCompSwap(missedBlock.spinLock,expect,1) != 0)
				expect = 0;
			int index = missedBlock.count++;
			missedBlock.blockId[index];
			// unlock
			missedBlock.spinLock = 0;
		}
		
		mapped = false;
		return 0.0;
	}
	else
	{
		vec3 samplePoint = pageTableEntry.xyz + blockOffset + repeatOffset;
		vec4 scalar = texture(cacheVolume,samplePoint);
		mapped = true;
		return scalar.r;
	}
}

vec3 PhongShading(vec3 samplePos, vec3 diffuseColor)
{
	vec3 shadedValue = vec3(0, 0, 0);

	vec3 N;
	N.x = (texture(texVolume, samplePos+vec3(step,0,0) ).w - texture(texVolume, samplePos+vec3(-step,0,0) ).w) - 1.0;
	N.y = (texture(texVolume, samplePos+vec3(0,step,0) ).w - texture(texVolume, samplePos+vec3(0,-step,0) ).w) - 1.0;
	N.z = (texture(texVolume, samplePos+vec3(0,0,step) ).w - texture(texVolume, samplePos+vec3(0,0,-step) ).w) - 1.0;

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

	vec3 rayStart = texture2DRect(texStartPos, textureRectCoord).xyz;
	vec3 rayEnd = texture2DRect(texEndPos, textureRectCoord).xyz;

	vec3 start2end = rayEnd - rayStart;
	vec4 bg = vec4(0.45f, 0.55f, 0.60f, 1.00f);
	if (start2end.x == 0.0 && start2end.y == 0.0 && start2end.z == 0.0) {
		fragColor = bg; // Background Colors
		return;
	}

	vec4 color = vec4(0.0,0.0,0.0,0.0);
	vec3 direction = normalize(start2end);
	float distance = dot(direction, start2end);
	int steps = int(distance / step);
	for (int i = 0; i < steps; ++i) {
		vec3 samplePoint = rayStart + direction * step * (float(i) + 0.5);
		vec4 scalar = texture(texVolume, samplePoint);
		vec4 sampledColor = texture(texTransfunc, scalar.r);
		sampledColor.rgb = PhongShading(samplePoint, sampledColor.rgb);
		color = color + sampledColor * vec4(sampledColor.aaa, 1.0) * (1.0 - color.a);
		if (color.a > 0.99)
			break;
	}
	if (color.a == 0.0)discard;
	

	color = color + vec4(bg.rgb, 0.0) * (1.0 - color.a);
	color.a = 1.0;
	fragColor = color;
}
