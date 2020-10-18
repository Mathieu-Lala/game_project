#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 OutColor;

uniform mat4 model;
uniform mat4 viewProj;

void main()
{
	gl_Position = viewProj * model * vec4(aPos, 1.0f);
	OutColor = aColor;
}
