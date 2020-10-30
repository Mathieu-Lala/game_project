#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 OutColor;

uniform mat4 model;
uniform mat4 viewProj;

uniform bool shake;
uniform float time;

void main()
{
    OutColor = aColor;

    gl_Position = viewProj * model * vec4(aPos, 1.0f);
    if (shake) {
        float strength = 0.01;
        gl_Position.x += cos(time / 1000 * 10) * strength;
        gl_Position.y += cos(time / 1000 * 15) * strength;
    }
}
