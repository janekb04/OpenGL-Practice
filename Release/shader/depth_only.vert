#version 330 core
layout (location = 0) in vec3 pos_;

uniform mat4 MVP_;

void main()
{
    gl_Position = MVP_ * vec4(pos_, 1.0);
}  