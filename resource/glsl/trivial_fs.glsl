#version 330 core
in vec3 frag_normal;
in vec3 frag_pos;
out vec4 frag_color;

uniform vec3 light_pos;
uniform vec3 light_color;
uniform vec4 object_color;
uniform vec3 view_pos;

void main()
{
    vec3 frag_norm = normalize(vec3(-frag_normal));

    //frag_color = vec4(frag_norm,1.0);
	
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
    float spec = pow(max(dot(light_reflect_dir,view_norm),0.0),64);

    vec4 specular = 0.5*spec*vec4(light_color,1.0);
    vec4 result = (ambient+diffuse+specular)*object_color;
    //fragment output
	frag_color = result;
}
