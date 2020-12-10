#version 330 core

layout (location = 0) in vec3 pos_;
layout (location = 1) in vec3 normal_;
layout (location = 2) in vec3 tangent_;
layout (location = 3) in vec2 uv_;

out vec3 pos;
out vec4 pos_in_light_space;
out vec3 normal;
out vec2 uv;
out vec3 view_dir;
out	vec3 light_dir;

uniform mat4 MVP_;
uniform mat4 MV_;
uniform mat4 N_;
uniform mat4 light_mat_;
uniform vec3 light_direction_;

void main()
{
	pos = (MV_ * vec4(pos_, 1.0)).xyz;
	pos_in_light_space = light_mat_ * vec4(pos_, 1.0);
	normal =  (N_ * vec4(normal_, 1.0)).xyz;
	vec3 tangent = (N_ * vec4(tangent_, 1.0)).xyz;
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN_inverse = transpose(mat3(tangent, bitangent, normal));
	view_dir = normalize(TBN_inverse * -pos);
	light_dir = normalize(TBN_inverse * -light_direction_);

	uv = uv_;
	gl_Position = MVP_ * vec4(pos_, 1.0);
}
