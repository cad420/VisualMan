
#version 430 core

layout(binding = 1, r32ui) uniform uimage2DRect headPointerImage;

//layout(binding = 2, rgba32ui) uniform uimageBuffer listBuffers;
layout(std430, binding =3) buffer ListBuffer{uvec4 buf[];}listBuffers;
layout(location = 0)out vec4 fragColor;


#define MAX_FRAGMENTS 15
uvec4 fragments[MAX_FRAGMENTS];


int buildLocalFragmentList(){
	int fragCount = 0;

	uint current = imageLoad(headPointerImage,ivec2(gl_FragCoord.xy)).r;

	while(current != 0xFFFFFFFF && fragCount < MAX_FRAGMENTS)
	{
		//uvec4 item = imageLoad(listBuffers,int(current));
		uvec4 item = listBuffers.buf[current];
		current = item.x;
		fragments[fragCount] = item;
		fragCount++;
	}

	return fragCount;

}


void sortFragmentList(int fragCount){
	int i,j;
	for(i = 0 ; i <fragCount;i++){
		bool move = false;
		for(j = i+1;j<fragCount;j++){
			float iDepth = uintBitsToFloat(fragments[i].z);
			float jDepth = uintBitsToFloat(fragments[j].z);
			if(iDepth > jDepth){
				move = true;
				uvec4 temp = fragments[i];
				fragments[i] = fragments[j];
				fragments[j] = temp;
			}
		}
		if(!move)
			break;
	}
}

vec4 blend(vec4 currentColor,vec4 newColor){
	return mix(currentColor,newColor,newColor.a);
}

vec4 calculateFinalColor(int fragCount){
	vec4 finalColor = vec4(0.0);

	for(int i = 0 ; i < fragCount;i++){
		vec4 fColor = unpackUnorm4x8(fragments[i].y);

		finalColor = blend(finalColor,fColor);
	}

	return finalColor;
}

void main(void){
	int fragCount = buildLocalFragmentList();
	sortFragmentList(fragCount);
	fragColor = calculateFinalColor(fragCount);
}