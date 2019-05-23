#version 330 core

// texture
uniform sampler1D texTransfunc;
uniform sampler3D texVolume;

//
uniform vec3 eye_position;
uniform vec3 light_position;


// illumination
uniform float step;
uniform float ka;
uniform float kd;
uniform float shininess;
uniform float ks;
uniform vec3 lightdir;
uniform vec3 halfway;

//in vec2 textureRectCoord;
in vec3 frag_position;

out vec4 fragColor;

vec3 PhongShading(vec3 samplePos, vec3 diffuseColor)
{
	vec3 shadedValue = vec3(0, 0, 0);

	vec3 N;
	N.x = (texture(texVolume, samplePos+vec3(step,0,0) ).w - texture(texVolume, samplePos+vec3(-step,0,0) ).w) - 1.0;
	N.y = (texture(texVolume, samplePos+vec3(0,step,0) ).w - texture(texVolume, samplePos+vec3(0,-step,0) ).w) - 1.0;
	N.z = (texture(texVolume, samplePos+vec3(0,0,step) ).w - texture(texVolume, samplePos+vec3(0,0,-step) ).w) - 1.0;
	//vec3 N = texture(texVolume, samplePos).xyz;
	N = N * 2.0 - 1.0;
	N = -normalize(N);
	//vec3 L = lightPos;
	//vec3 V = viewDir;
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
	vec3 rayStart = eye_position;
	vec3 rayEnd = frag_position;
	vec3 start2end = rayEnd - rayStart;
	vec4 bg = vec4(0,0,0, 1.0);
	vec4 color = vec4(0, 0, 0, 0);
	vec3 direction = normalize(start2end);
	vec3 samplePoint = frag_position;
	for (int i = 0;;++i) 
	{
		samplePoint +=  direction * step ;
		//fragColor = vec4(samplePoint,1.0);
		if (any(greaterThan(samplePoint,vec3(1,1,1))))break;
		if (any(lessThan(samplePoint,vec3(0,0,0))))break;

		if(samplePoint.x >= 1.0f || samplePoint.y >=1.0f || samplePoint.z >= 1.0f)
			break;

		vec4 scalar = texture(texVolume, samplePoint);
		vec4 sampledColor = texture(texTransfunc, scalar.r);
		color = color + sampledColor * vec4(sampledColor.aaa, 1.0) * (1.0 - color.a);
		if (color.a > 0.99)
			break;
	}
	if (color.a == 0.0) 
		discard;
	color = color + vec4(bg.rgb, 0.0) * (1.0 - color.a);
	color.a = 1.0;
	fragColor = color;
}