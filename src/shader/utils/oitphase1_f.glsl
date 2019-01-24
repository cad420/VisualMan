#version 430 core


//layout(early_fragment_tests) in;

layout(binding = 0 ,offset = 0) uniform atomic_uint indexCounter;
layout(binding = 1, r32ui) uniform uimage2DRect headPointerImage;
layout(binding = 2, rgba32ui) uniform uimageBuffer listBuffers;

// Phong Shading
in vec3 frag_normal;
in vec3 frag_pos;

uniform vec3 light_pos;
uniform vec3 light_color;
uniform vec4 object_color;
uniform vec3 view_pos;

out vec4 frag_color;

vec4 PhongShading()
{
    vec3 frag_norm = normalize(frag_normal);

	
   // the light normalized vector points to light
    vec3 light_norm = normalize(light_pos-frag_pos);

    //ambient
    vec4 ambient = 0.1*vec4(light_color,1.0);

   // diffuse
    float diff_strength = max(dot(light_norm,frag_norm),0.0);
    vec4 diffuse = diff_strength*vec4(light_color,1.0);

    //specular
    vec3 view_norm = normalize(view_pos-frag_pos);
    vec3 light_reflect_dir = reflect(-light_norm,frag_norm);
    float spec = pow(max(dot(light_reflect_dir,view_norm),0.0),32);

    vec4 specular = 0.5*spec*vec4(light_color,1.0);
    vec4 result = (ambient+diffuse+specular)*object_color;
    //fragment output
	//frag_color = vec4(result, 1.0);
	return result;
}




// Phong Shading End

void main(void){

	// TODO::
	vec4 fragColor = PhongShading();

	uint newHead = atomicCounterIncrement(indexCounter);
	uint oldHead = imageAtomicExchange(headPointerImage,ivec2(gl_FragCoord.xy),newHead);

	uvec4 item;
	item.x = oldHead;
	item.y = packUnorm4x8(fragColor);
	item.z = floatBitsToUint(gl_FragCoord.z);
	item.w = 0;

	imageStore(listBuffers,int(newHead),item);

}