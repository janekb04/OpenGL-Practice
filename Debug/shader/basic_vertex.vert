#version 330 core

layout (location = 0) in vec3 pos_;
layout (location = 1) in vec3 color_;
layout (location = 2) in vec2 uv_;

out vec2 uv;

void main()
{
	uv = uv_;
	gl_Position = vec4(pos_, 1.0);
}
