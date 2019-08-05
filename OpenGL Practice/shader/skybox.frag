#version 330 core

out vec4 color;

in vec3 position;

uniform samplerCube skybox_;

void main()
{
	color = texture(skybox_, position);
}