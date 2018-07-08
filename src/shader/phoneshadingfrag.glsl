#version 330 core
in vec3 frag_normal;
in vec3 frag_pos;
uniform vec3 light_pos;
uniform vec3 light_color;
uniform vec3 object_color;
uniform vec3 view_pos;
out vec4 frag_color;
void main()
{
    vec3 frag_norm = normalize(frag_normal);
   // the light normalized vector points to light
    vec3 light_norm = normalize(light_pos-frag_pos);

    //ambient
    vec3 ambient = 0.1*light_color;

   // diffuse
    float diff_strength = max(dot(light_norm,frag_norm),0.0);
    vec3 diffuse = diff_strength*light_color;

    //specular
    vec3 view_norm = normalize(view_pos-frag_pos);
    vec3 light_reflect_dir = reflect(-light_norm,frag_norm);
    float spec = pow(max(dot(light_reflect_dir,view_norm),0.0),32);

    vec3 specular = 0.5*spec*light_color;
    vec3 result = (ambient+diffuse+specular)*object_color;
    //fragment output
	frag_color = vec4(result, 1.0);

	//frag_color = vec4(object_color,1.0f);
}
