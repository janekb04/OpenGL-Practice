#version 330 core

out vec4 frag_color;

in vec2 uv;

uniform sampler2D diffuse_;

void main()
{
	frag_color = texture(diffuse_, uv);
}
