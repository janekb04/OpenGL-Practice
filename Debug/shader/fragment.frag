#version 330 core

out vec4 frag_color;

in vec3 pos;
in vec4 pos_in_light_space;
in vec3 normal;
in vec2 uv;
in vec3 view_dir;
in vec3 light_dir;

uniform vec3 ambient_color_;
uniform vec3 light_color_;

uniform sampler2D diffuse_;
uniform sampler2D specular_;
uniform sampler2D normal_map_;
uniform sampler2DShadow shadow_map_;
uniform float shininess_;

void main()
{
	vec3 N = normalize(texture(normal_map_, uv).rgb * 2.0 - 1.0);
	vec3 halfway_dir = normalize(light_dir + view_dir);

	//vec3 to_light = light_position_ - pos;
	//float attenuation = 1.0 / (to_light.x * to_light.x + to_light.y * to_light.y + to_light.z * to_light.z);  

	vec3 ambient = ambient_color_;
	float cos_theta = dot(light_dir, N);

	float diff = max(0, cos_theta);
	vec3 diffuse = diff * light_color_; // * attenuation;

	float spec = pow(max(dot(N, halfway_dir), 0.0), shininess_);// * float(diff > 0.01);
	vec3 specular = light_color_ * spec * texture(specular_, uv).rgb; // * attenuation;
	
	vec3 light_space_coords = pos_in_light_space.xyz / pos_in_light_space.w;
	light_space_coords = light_space_coords * 0.5 + 0.5; 
	//float bias = 0.0017 * (1.0 - cos_theta * cos_theta);  
	float shadow = texture(shadow_map_, light_space_coords);

	vec3 lighting = ambient + shadow * (diffuse + specular);
	frag_color = vec4(texture(diffuse_, uv).rgb * lighting, 1.0);
	frag_color.rgb = pow(frag_color.rgb, vec3(1.0f / 2.2f));
}
