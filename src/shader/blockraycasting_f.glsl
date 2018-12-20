#version 420

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

uniform sampler3D cacheVolume;
uniform ivec3 pageDirSize;
uniform ivec3 pageTableSize;
uniform ivec3 pageTableEntrySize;
uniform ivec3 volumeDataSize;
uniform ivec3 blockDataSize;
uniform ivec3 repeatSize;

layout(binding = 0, rgba32i) uniform iimage3D pageDirTexutre;
layout(binding = 1, rgba32i) uniform iimage3D pageTableTexture;
layout(binding = 2, r32i) uniform iimage1D cacheMissedTexture;

float virtualVolumeSample(vec3 samplePos)
{
	ivec3 pDirAddress = ivec3(samplePos*pageDirSize);
	ivec4 pageDirEntry = imageLoad(pageDirTexutre,pDirAddress);
	ivec3 pTableAddress = ivec3(pageDirEntry) + ivec3(samplePos*pageTableSize) % pageTableEntrySize;
	ivec4 pageTableEntry = imageLoad(pageTableTexture,pTableAddress);
	if(pageTableEntry.w == 2)		// Unmapped flag
	{
		
	}else{
		vec3 samplePoint = pageTableEntry.xyz + ivec3(samplePos * volumeDataSize) % blockDataSize;
		vec4 scalar = texture(cacheVolume,samplePoint);

	}
	return 1.0;
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
	vec4 bg = vec4(.2, .3, .4, 1.0);
	if (start2end.x == 0.0 && start2end.y == 0.0 && start2end.z == 0.0) {
		fragColor = bg; // Background Colors
		return;
	}

	vec4 color = vec4(0, 0, 0, 0);
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
	if (color.a == 0.0) 
		return;
	color = color + vec4(bg.rgb, 0.0) * (1.0 - color.a);
	color.a = 1.0;
	fragColor = color;
}
